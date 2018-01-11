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
   */
  public function testTokenHookAlteredBySetting() {
    $tokens = array();
    $this->callAPISuccess('Setting', 'create', array('civitoken_enabled_tokens' => array('address.address_block')));
    civitoken_civicrm_tokens($tokens);
    $this->assertEquals(['address' => ['address.address_block' => 'Address Block']], $tokens);
  }
}
