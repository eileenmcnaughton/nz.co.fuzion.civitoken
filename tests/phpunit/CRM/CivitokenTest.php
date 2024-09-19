<?php

use Civi\Test\Api3TestTrait;
use Civi\Test\CiviEnvBuilder;
use Civi\Test\HeadlessInterface;
use Civi\Test\HookInterface;
use Civi\Test\TransactionalInterface;
use Civi\Token\TokenProcessor;
use Civi\CiviTokens;
use PHPUnit\Framework\TestCase;

require_once 'BaseUnitTestClass.php';

/**
 * FIXME - Add test description.
 *
 * Tips:
 *  - With HookInterface, you may implement CiviCRM hooks directly in the test class.
 *    Simply create corresponding functions (e.g. "hook_civicrm_post(...)" or similar).
 *  - With TransactionalInterface, any data changes made by setUp() or test****() functions will
 *    rollback automatically -- as long as you don't manipulate schema or truncate tables.
 *    If this test needs to manipulate schema or truncate tables, then either:
 *       a. Do all that using setupHeadless() and Civi\Test.
 *       b. Disable TransactionalInterface, and handle all setup/teardown yourself.
 *
 * @group headless
 */
class CRM_CivitokenTest extends TestCase implements HeadlessInterface, HookInterface, TransactionalInterface {

  use Api3TestTrait;
  public $ids;

  /**
   * Set up for headless tests.
   *
   * @return \Civi\Test\CiviEnvBuilder
   * @throws \CRM_Extension_Exception_ParseException
   */
  public function setUpHeadless(): CiviEnvBuilder {
    // Civi\Test has many helpers, like install(), uninstall(), sql(), and sqlFile().
    // See: https://github.com/civicrm/org.civicrm.testapalooza/blob/master/civi-test.md
    return \Civi\Test::headless()
      ->installMe(__DIR__)
      ->apply();
  }

  /**
   * Test token hook function works.
   *
   * @throws \CRM_Core_Exception
   */
  public function testTokenHook(): void {
    $processor = new \Civi\Token\CiviTokens();
    $tokens = $processor->getTokenMetadata();

    $this->assertNotEmpty($tokens);
    $this->assertEquals('Address Block', $tokens['address']['address.address_block']);
  }

  /**
   * Test token hook function is limited if a setting is used.
   *
   * @throws \CRM_Core_Exception
   */
  public function testTokenHookAlteredBySetting(): void {
    $this->callAPISuccess('Setting', 'create', ['civitoken_enabled_tokens' => ['address.address_block']]);
    $processor = new \Civi\Token\CiviTokens();
    $tokens = $processor->getTokenMetadata();
    $this->assertEquals(['address' => ['address.address_block' => 'Address Block']], $tokens);
  }

  /**
   * Test whether the relationship tokens work.
   *
   * @throws \CRM_Core_Exception
   */
  public function testRelationShipTokens(): void {
    $tokens = $this->getTokenProcessorTokens();

    $relationships = relationships_get_relationship_list();
    foreach ($relationships as $id => $label) {
      $this->assertEquals($label . ' : Name of first contact found', $tokens['{relationships.display_name_' . $id . '}']);
      $this->assertEquals($label . ' : First Name of first contact found', $tokens['{relationships.first_name_' . $id . '}']);
      $this->assertEquals($label . ' : Last Name of first contact found', $tokens['{relationships.last_name_' . $id. '}']);
      $this->assertEquals($label . ' : Phone of first contact found', $tokens['{relationships.phone_' . $id. '}']);
      $this->assertEquals($label . ' : Email of first contact found', $tokens['{relationships.email_' . $id. '}']);
      $this->assertEquals($label . ' : ID of first contact found', $tokens['{relationships.id_' . $id. '}']);
    }
  }

  /**
   * Test token hook function is limited if a setting is used in this case for
   * relationship.
   *
   * @throws \CRM_Core_Exception
   */
  public function testRelationShipTokensAlteredBySettings(): void {
    $tokens = [];
    $tokens_to_enable = [];
    $relationships = relationships_get_relationship_list();
    foreach ($relationships as $id => $label) {
      $tokens_to_enable[] = 'relationships.first_name_' . $id;
    }
    $this->callAPISuccess('Setting', 'create', ['civitoken_enabled_tokens' => $tokens_to_enable]);
    $processor = new \Civi\Token\CiviTokens();
    $tokens = $processor->getTokenMetadata();
    foreach ($relationships as $id => $label) {
      $this->assertEquals($label . ' : First Name of first contact found', $tokens['relationships']['relationships.first_name_' . $id]);
    }
  }

  /**
   * Test contribution tokens for a contact with no contributions.
   *
   * This tests that regression from
   * https://github.com/eileenmcnaughton/nz.co.fuzion.civitoken/pull/18 whereby
   * an exception was thrown for contact without contributions stays fixed.
   *
   */
  public function testContributionTokenNull(): void {
    $contributionTokens = [
      'latestcontribs.softcredit_name',
      'latestcontribs.softcredit_type',
    ];
    $this->individualCreate();
    $this->callAPISuccess('Setting', 'create', ['civitoken_enabled_tokens' => $contributionTokens]);
    $this->ids['contact'][0] = $this->individualCreate();
    $this->ids['contact'][1] = $this->individualCreate();
    $this->ids['contribution'][0] = $this->callAPISuccess('Contribution', 'create', [
      'api.ContributionSoft.create' => [
        'amount' => 5,
        'contact_id' => $this->ids['contact'][1],
        'soft_credit_type_id' => 'in_memory_of',
      ],
      'total_amount' => 10,
      'contact_id' => $this->ids['contact'][0],
      'financial_type_id' => 'Donation',
    ]);
    $rendered = $this->processTokens(['latestcontribs.softcredit_name', 'latestcontribs.softcredit_type']);
    $this->assertEquals('bob In Memory of',  $rendered);
  }

  /**
   * Test that no fatal error occurs when all tokens are requested.
   */
  public function testNoFatal(): void {
    $this->ids['contact'][0] = $this->individualCreate();
    $tokens = $this->getTokenProcessorTokens();
    $string = '';
    foreach ($tokens as $token => $label) {
      $string.= $label . ': ' . $token . "\n";
    }

    // This is a test  it doesn't matter if it's not supported - everything that IS supported is hard to use.
    $rendered = $this->render(['text' => $string]);
    $this->assertStringContainsString('Communication Style: Formal', $rendered['text']);
  }

  /**
   * Render some template(s), evaluating token expressions and Smarty expressions.
   *
   * Copied from Core as it is internal in core so should not call....
   *
   * This helper simplifies usage of hybrid notation. As a simplification, it may not be optimal for processing
   * large batches (e.g. CiviMail or scheduled-reminders), but it's a little more convenient for 1-by-1 use-cases.
   *
   * @param array $messages
   *   Message templates. Any mix of the following templates ('text', 'html', 'subject', 'msg_text', 'msg_html', 'msg_subject').
   *   Ex: ['subject' => 'Hello {contact.display_name}', 'text' => 'What up?'].
   *   Note: The content-type may be inferred by default. A key like 'html' or 'msg_html' indicates HTML formatting; any other key indicates text formatting.
   * @param array $tokenContext
   *   Ex: ['contactId' => 123, 'activityId' => 456]
   * @param array|null $smartyAssigns
   *   List of data to export via Smarty.
   *   Data is only exported temporarily (long enough to execute this render() method).
   * @return array
   *   Rendered messages. These match the various inputted $messages.
   *   Ex: ['msg_subject' => 'Hello Bob Roberts', 'msg_text' => 'What up?']
   * @internal
   */
  protected function render(array $messages, $format = 'text/html'): array {
    $result = [];
    $tokenContextDefaults = [
      'controller' => __CLASS__,
      'smarty' => TRUE,
      'schema' => ['contactId'],
    ];
    $tokenProcessor = new TokenProcessor(\Civi::dispatcher(), $tokenContextDefaults);
    $tokenProcessor->addRow(['contactId' => $this->ids['contact'][0]]);

    // Load templates
    foreach ($messages as $messageId => $messageTpl) {
      $tokenProcessor->addMessage($messageId, $messageTpl, $format);
    }

    // Evaluate/render templates
    $tokenProcessor->evaluate();
    foreach ($messages as $messageId => $ign) {
      foreach ($tokenProcessor->getRows() as $row) {
        $result[$messageId] = $row->render($messageId);
      }
    }
    return $result;
  }

  /**
   * Test address token renders state.
   */
  public function testAddressToken(): void {
    $this->ids['contact'][0] = $this->individualCreate();
    $this->callAPISuccess('Address', 'create', [
      'contact_id' => $this->ids['contact'][0],
      'city' => 'Baltimore',
      'state_province_id' => 'Maine',
      'country_id' => 'US',
    ]);
    $tokens = ['address.address_block'];
    $rendered = $this->processTokens($tokens);
    $this->assertEquals('bob<br />
Baltimore, ME<br />
UNITED STATES<br />
', $rendered);
    $this->callAPISuccess('Setting', 'create', [
      'mailing_format' => '{contact.addressee}
{contact.street_address}
{contact.supplemental_address_1}
{contact.city}
{contact.state_province_name}
{contact.postal_code}
{contact.country}',
    ]);
    $rendered = $this->processTokens($tokens, 'text/plain');
    $this->assertEquals('bob
Baltimore
Maine
UNITED STATES
', $rendered);
  }

  /**
   * Create a test individual.
   *
   * @return int
   *
   */
  protected function individualCreate(): int {
    return (int) $this->callAPISuccess('Contact', 'create', [
      'contact_type' => 'Individual',
      'first_name' => 'bob',
    ])['id'];
  }

  /**
   * Process tokens through the hook.
   *
   * @param array $tokens
   *
   * @return string
   */
  protected function processTokens(array $tokens, $format = 'text/html'): string {
    $this->callAPISuccess('Setting', 'create', ['civitoken_enabled_tokens' => $tokens]);
    Civi::cache()->delete('civitoken_enabled_tokens');
    $text = '{' . implode('} {' , $tokens) . '}';
    return $this->render(['text' => $text], $format)['text'];
  }

  /**
   * Get the tokens declared via the token processor.
   *
   * @return array
   */
  protected function getTokenProcessorTokens(): array {
    $tokenProcessor = new TokenProcessor(\Civi::dispatcher(), [
      'schema' => ['contactId'],
    ]);
    return $tokenProcessor->listTokens();
  }

  /**
   * @param string $message
   *
   * @return string
   */
  protected function getRendered(string $message): string {
    $tokenProcessor = new TokenProcessor(\Civi::dispatcher(), []);
    $tokenProcessor->addRow(['contactId' => [$this->ids['contact'][0]]]);
    $tokenProcessor->addMessage('text', $message, 'text/html');
    $tokenProcessor->evaluate();
    // Display mail-merge data.
    foreach ($tokenProcessor->getRows() as $row) {
      return $row->render('text');
    }
  }

}
