<?php

use CRM_Civitoken_ExtensionUtil as E;
use Civi\Test\HeadlessInterface;
use Civi\Test\HookInterface;
use Civi\Test\TransactionalInterface;
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
class CRM_CivitokenTest extends BaseUnitTestClass implements HeadlessInterface, HookInterface, TransactionalInterface {

  public $ids;

  /**
   * Set up for headless tests.
   *
   * @return \Civi\Test\CiviEnvBuilder
   * @throws \CRM_Extension_Exception_ParseException
   */
  public function setUpHeadless() {
    // Civi\Test has many helpers, like install(), uninstall(), sql(), and sqlFile().
    // See: https://github.com/civicrm/org.civicrm.testapalooza/blob/master/civi-test.md
    return \Civi\Test::headless()
      ->installMe(__DIR__)
      ->apply();
  }

  public function setUp() {
    parent::setUp();
  }

  public function tearDown() {
    parent::tearDown();
  }

  /**
   * Test token hook function works.
   */
  public function testTokenHook() {
    $tokens = array();
    civitoken_civicrm_tokens($tokens);
    $this->assertTrue(!empty($tokens));
    $this->assertEquals('Address Block', $tokens['address']['address.address_block']);
  }

  /**
   * Test token hook function is limited if a setting is used.
   *
   * @throws \CRM_Core_Exception
   * @throws \CiviCRM_API3_Exception
   */
  public function testTokenHookAlteredBySetting() {
    $tokens = [];
    $this->callAPISuccess('Setting', 'create', array('civitoken_enabled_tokens' => array('address.address_block')));
    civitoken_civicrm_tokens($tokens);
    $this->assertEquals(['address' => ['address.address_block' => 'Address Block']], $tokens);
  }

  /**
   * Test whether the relationship tokens work.
   *
   * @throws \CiviCRM_API3_Exception
   */
  public function testRelationShipTokens() {
    $tokens = [];
    civitoken_civicrm_tokens($tokens);
    $this->assertNotEmpty($tokens);

    $relationships = relationships_get_relationship_list();
    foreach($relationships as $id => $label) {
      $this->assertEquals($label . ' : Name of first contact found', $tokens['relationships']['relationships.display_name_' . $id]);
      $this->assertEquals($label . ' : First Name of first contact found', $tokens['relationships']['relationships.first_name_' . $id]);
      $this->assertEquals($label . ' : Last Name of first contact found', $tokens['relationships']['relationships.last_name_' . $id]);
      $this->assertEquals($label . ' : Phone of first contact found', $tokens['relationships']['relationships.phone_' . $id]);
      $this->assertEquals($label . ' : Email of first contact found', $tokens['relationships']['relationships.email_' . $id]);
      $this->assertEquals($label . ' : ID of first contact found', $tokens['relationships']['relationships.id_' . $id]);
    }
  }

  /**
   * Test token hook function is limited if a setting is used in this case for relationship.
   *
   * @throws \CiviCRM_API3_Exception
   * @throws \CRM_Core_Exception
   */
  public function testRelationShipTokensAlteredBySettings() {
    $tokens = [];
    $tokens_to_enable = [];
    $relationships = relationships_get_relationship_list();
    foreach($relationships as $id => $label) {
      $tokens_to_enable[] = 'relationships.first_name_'.$id;
    }
    $this->callAPISuccess('Setting', 'create', ['civitoken_enabled_tokens' => $tokens_to_enable]);
    civitoken_civicrm_tokens($tokens);
    foreach($relationships as $id => $label) {
      $this->assertEquals($label . ' : First Name of first contact found', $tokens['relationships']['relationships.first_name_' . $id]);
    }
  }

  /**
   * Test contribution tokens for a contact with no contributions.
   *
   * This tests that regression from https://github.com/eileenmcnaughton/nz.co.fuzion.civitoken/pull/18
   * whereby an exception was thrown for contact without contributions stays fixed.
   *
   * @throws \CRM_Core_Exception
   */
  public function testContributionTokenNull() {
    $contributionTokens = [
      'latestcontribs.softcredit_name',
      'latestcontribs.softcredit_type',
    ];
    $this->individualCreate();
    $this->callAPISuccess('Setting', 'create', ['civitoken_enabled_tokens' => $contributionTokens]);
    $this->ids['contact'][0] = $this->individualCreate();
    $this->ids['contact'][1] = $this->individualCreate();
    $this->ids['contribution'][0] = $this->callAPISuccess('Contribution', 'create', [
      'api.ContributionSoft.create' => ['amount' => 5, 'contact_id' => $this->ids['contact'][1], 'soft_credit_type_id' => 'in_memory_of'],
      'total_amount' => 10,
      'contact_id' => $this->ids['contact'][0],
      'financial_type_id' => 'Donation',
    ]);
    $values = [];
    civitoken_civicrm_tokenValues($values, [$this->ids['contact'][0]], NULL, ['latestcontribs' => ['softcredit_name', 'softcredit_type']]);
    $this->assertEquals('In Memory of', $values[$this->ids['contact'][0]]['latestcontribs.softcredit_type']);
  }

  /**
   * Test address token renders state.
   *
   * @throws \CRM_Core_Exception
   */
  public function testAddressToken() {
    $this->ids['contact'][0] = $this->individualCreate();
    $this->callAPISuccess('Address', 'create', ['contact_id' => $this->ids['contact'][0], 'city' => 'Baltimore', 'state_province_id' => 'Maine', 'country_id' => 'US']);
    $tokens = ['address.address_block'];
    $values = $this->processTokens($tokens);
    $this->assertEquals([
      'address.address_conditional_country' => 'UNITED STATES',
      'address.address_block_text' => 'bob
Baltimore, ME
UNITED STATES
',
      'address.address_block' => 'bob<br />
Baltimore, ME<br />
UNITED STATES<br />
',
    ], $values[$this->ids['contact'][0]]
    );
    $this->callAPISuccess('Setting', 'create', ['mailing_format' => '{contact.addressee}
{contact.street_address}
{contact.supplemental_address_1}
{contact.city}
{contact.state_province_name}
{contact.postal_code}
{contact.country}']);
    $values = $this->processTokens($tokens);
    $this->assertEquals('bob
Baltimore
Maine
UNITED STATES
', $values[$this->ids['contact'][0]]['address.address_block_text']);
  }

  /**
   * Create a test individual.
   *
   * @return int
   *
   * @throws \CRM_Core_Exception
   */
  protected function individualCreate(): int {
    return (int) $this->callAPISuccess('Contact', 'create', ['contact_type' => 'Individual', 'first_name' => 'bob'])['id'];
  }

  /**
   * Process tokens through the hook.
   *
   * @param array $tokens
   *
   * @return array
   * @throws \CRM_Core_Exception
   */
  protected function processTokens(array $tokens): array {
    $this->callAPISuccess('Setting', 'create', ['civitoken_enabled_tokens' => $tokens]);
    $values = [];
    $parsedTokens = [];
    foreach ($tokens as $token) {
      $split = explode('.', $token);
      $parsedTokens[$split[0]] = $split[1];
    }
    \Civi::cache()->delete('civitoken_enabled_tokens');
    civitoken_civicrm_tokenValues($values, [$this->ids['contact'][0]], NULL, $parsedTokens);
    return $values;
  }

}
