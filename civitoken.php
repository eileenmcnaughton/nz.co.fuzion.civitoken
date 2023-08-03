<?php

use Civi\Token\CiviTokens;
use Symfony\Component\Config\Resource\FileResource;
use Symfony\Component\DependencyInjection\ContainerBuilder;

require_once 'civitoken.civix.php';
// phpcs:disable
use CRM_Civitoken_ExtensionUtil as E;
use Symfony\Component\DependencyInjection\Definition;

// phpcs:enable

/**
 * Implements hook_civicrm_config().
 *
 * @link https://docs.civicrm.org/dev/en/latest/hooks/hook_civicrm_config/
 */
function civitoken_civicrm_config(&$config) {
  _civitoken_civix_civicrm_config($config);
}

/**
 * Implements hook_civicrm_install().
 *
 * @link https://docs.civicrm.org/dev/en/latest/hooks/hook_civicrm_install
 */
function civitoken_civicrm_install() {
  _civitoken_civix_civicrm_install();
}

/**
 * Implements hook_civicrm_enable().
 *
 * @link https://docs.civicrm.org/dev/en/latest/hooks/hook_civicrm_enable
 */
function civitoken_civicrm_enable() {
  _civitoken_civix_civicrm_enable();
}

// --- Functions below this ship commented out. Uncomment as required. ---

/**
 * Implements hook_civicrm_preProcess().
 *
 * @link https://docs.civicrm.org/dev/en/latest/hooks/hook_civicrm_preProcess
 */
//function civitoken_civicrm_preProcess($formName, &$form) {
//
//}

/**
 * Implements hook_civicrm_navigationMenu().
 *
 * @link https://docs.civicrm.org/dev/en/latest/hooks/hook_civicrm_navigationMenu
 */
function civitoken_civicrm_navigationMenu(&$menu) {

  _civitoken_civix_insert_navigation_menu($menu, 'Administer/Communications', [
    'label' => ts('Enabled Tokens', ['domain' => 'nz.co.fuzion.civitoken']),
    'name' => 'enabled_civitokens',
    'url' => 'civicrm/a/#/civitoken/settings',
    'permission' => 'administer CiviCRM',
    'operator' => 'OR',
    'separator' => 0,
  ]);
  _civitoken_civix_navigationMenu($menu);
}

/**
 * implementation of CiviCRM hook
 */
function civitoken_civicrm_tokens_all(&$tokens) {
  $tokenFunctions = civitoken_initialize();
  $civitokens = [];
  foreach ($tokenFunctions as $token) {
    $fn = $token . '_civitoken_declare';
    $tokens[$token] = array_merge($civitokens, $fn($token) ?? []);
  }
  $tokens['civitokens'] = $civitokens;
}

/**
 * Get a flattened list of tokens.
 *
 * e.g
 * ['address.address_block' => 'Address Block', 'date.today' => 'Today\'s date']].
 */
function civitoken_get_flattened_list_all() {
  $tokens = [];
  $flattenedTokens = [];
  civitoken_civicrm_tokens_all($tokens);
  foreach ($tokens as $category) {
    foreach ($category as $token => $title) {
      $flattenedTokens[$token] = $title;
    }
  }
  return $flattenedTokens;
}

/**
 * Gather functions from tokens in tokens folder
 */
function civitoken_initialize() {
  if (isset(Civi::$statics['civitoken']['tokens'])) {
    return Civi::$statics['civitoken']['tokens'];
  }
  $tokens = [];
  $config = CRM_Core_Config::singleton();
  $directories = [__DIR__ . '/tokens'];
  if (!empty($config->customPHPPathDir)) {
    if (file_exists($config->customPHPPathDir . '/tokens')) {
      $directories[] = $config->customPHPPathDir . '/tokens';
    }
  }
  // lookup extension directories
  foreach (explode(':', get_include_path()) as $path) {
    if (FALSE !== strpos($path, $config->extensionsDir) && file_exists($path . '/tokens')) {
      $directories[] = $path;
    }
  }
  foreach ($directories as $directory) {
    $tokenFiles = CRM_Utils_File::findFiles($directory, '*.inc');
    foreach ($tokenFiles as $file) {
      require_once $file;
      $re = "/.*\\/([a-z]*).inc/";
      preg_match($re, $file, $matches);
      $tokens[] = $matches[1];
    }
  }
  Civi::$statics['civitoken']['tokens'] = $tokens;
  return Civi::$statics['civitoken']['tokens'];
}

/**
 * Add token services to the container.
 *
 * @param \Symfony\Component\DependencyInjection\ContainerBuilder $container
 */
function civitoken_civicrm_container(ContainerBuilder $container) {
  $container->addResource(new FileResource(__FILE__));
  $container->setDefinition('crm_civitoken', new Definition(
    \Civi\Token\CiviTokens::class,
    []
  ))->addTag('kernel.event_subscriber')->setPublic(TRUE);
}
