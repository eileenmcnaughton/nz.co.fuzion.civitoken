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
    $tokens = array();
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
}
