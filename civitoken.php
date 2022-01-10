<?php

require_once 'civitoken.civix.php';
// phpcs:disable
use CRM_Civitoken_ExtensionUtil as E;
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
 * Implements hook_civicrm_xmlMenu().
 *
 * @link https://docs.civicrm.org/dev/en/latest/hooks/hook_civicrm_xmlMenu
 */
function civitoken_civicrm_xmlMenu(&$files) {
  _civitoken_civix_civicrm_xmlMenu($files);
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
 * Implements hook_civicrm_postInstall().
 *
 * @link https://docs.civicrm.org/dev/en/latest/hooks/hook_civicrm_postInstall
 */
function civitoken_civicrm_postInstall() {
  _civitoken_civix_civicrm_postInstall();
}

/**
 * Implements hook_civicrm_uninstall().
 *
 * @link https://docs.civicrm.org/dev/en/latest/hooks/hook_civicrm_uninstall
 */
function civitoken_civicrm_uninstall() {
  _civitoken_civix_civicrm_uninstall();
}

/**
 * Implements hook_civicrm_enable().
 *
 * @link https://docs.civicrm.org/dev/en/latest/hooks/hook_civicrm_enable
 */
function civitoken_civicrm_enable() {
  _civitoken_civix_civicrm_enable();
}

/**
 * Implements hook_civicrm_disable().
 *
 * @link https://docs.civicrm.org/dev/en/latest/hooks/hook_civicrm_disable
 */
function civitoken_civicrm_disable() {
  _civitoken_civix_civicrm_disable();
}

/**
 * Implements hook_civicrm_upgrade().
 *
 * @link https://docs.civicrm.org/dev/en/latest/hooks/hook_civicrm_upgrade
 */
function civitoken_civicrm_upgrade($op, CRM_Queue_Queue $queue = NULL) {
  return _civitoken_civix_civicrm_upgrade($op, $queue);
}

/**
 * Implements hook_civicrm_managed().
 *
 * Generate a list of entities to create/deactivate/delete when this module
 * is installed, disabled, uninstalled.
 *
 * @link https://docs.civicrm.org/dev/en/latest/hooks/hook_civicrm_managed
 */
function civitoken_civicrm_managed(&$entities) {
  _civitoken_civix_civicrm_managed($entities);
}

/**
 * Implements hook_civicrm_caseTypes().
 *
 * Generate a list of case-types.
 *
 * Note: This hook only runs in CiviCRM 4.4+.
 *
 * @link https://docs.civicrm.org/dev/en/latest/hooks/hook_civicrm_caseTypes
 */
function civitoken_civicrm_caseTypes(&$caseTypes) {
  _civitoken_civix_civicrm_caseTypes($caseTypes);
}

/**
 * Implements hook_civicrm_angularModules().
 *
 * Generate a list of Angular modules.
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_caseTypes
 */
function civitoken_civicrm_angularModules(&$angularModules) {
  _civitoken_civix_civicrm_angularModules($angularModules);
}

/**
 * Implements hook_civicrm_alterSettingsFolders().
 *
 * @link https://docs.civicrm.org/dev/en/latest/hooks/hook_civicrm_alterSettingsFolders
 */
function civitoken_civicrm_alterSettingsFolders(&$metaDataFolders = NULL) {
  _civitoken_civix_civicrm_alterSettingsFolders($metaDataFolders);
}

/**
 * Implements hook_civicrm_entityTypes().
 *
 * Declare entity types provided by this module.
 *
 * @link https://docs.civicrm.org/dev/en/latest/hooks/hook_civicrm_entityTypes
 */
function civitoken_civicrm_entityTypes(&$entityTypes) {
  _civitoken_civix_civicrm_entityTypes($entityTypes);
}

/**
 * Implements hook_civicrm_themes().
 */
function civitoken_civicrm_themes(&$themes) {
  _civitoken_civix_civicrm_themes($themes);
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
 *
 * @throws \CiviCRM_API3_Exception
 */
function civitoken_civicrm_tokens(&$tokens) {
  $civiTokens = \Civi::cache()->get('civitoken_enabled_tokens');
  if (!is_array($civiTokens)) {
    $civiTokens = [];
    civitoken_civicrm_tokens_all($civiTokens);
    $setting = civicrm_api3('Setting', 'get', [
      'return' => 'civitoken_enabled_tokens',
      'sequential' => 1,
    ])['values'][0];

    if (empty($setting) || empty($setting['civitoken_enabled_tokens'])) {
      // Treat un-configured as 'all enabled'.
      \Civi::cache()->set('civitoken_enabled_tokens', $civiTokens);
      $tokens = array_merge($tokens, $civiTokens);
      return;
    }

    foreach ($civiTokens as $category => $tokenSubset) {
      foreach ($tokenSubset as $key => $token) {
        if (!in_array($key, $setting['civitoken_enabled_tokens'])) {
          unset($civiTokens[$category][$key]);
        }
      }
      if (empty($civiTokens[$category])) {
        unset($civiTokens[$category]);
      }
    }
    \Civi::cache()->set('civitoken_enabled_tokens', $civiTokens);
  }
  $tokens = array_merge($tokens, $civiTokens);

}

/**
 * implementation of CiviCRM hook
 */
function civitoken_civicrm_tokens_all(&$tokens) {
  $tokenFunctions = civitoken_initialize();
  $civitokens = [];
  foreach ($tokenFunctions as $token) {
    $fn = $token . '_civitoken_declare';
    $tokens[$token] = array_merge($civitokens, $fn($token));
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
 * implementation of CiviCRM hook
 *
 * @param array $values
 * @param $contactIDs
 * @param null $job
 * @param array $tokens
 * @param null $context
 */
function civitoken_civicrm_tokenValues(&$values, $contactIDs, $job = NULL, $tokens = [], $context = NULL) {
  $tokenFunctions = civitoken_initialize();
  // @todo figure out full conditions for returning here.
  if (empty($tokens) || (array_keys($tokens) === ['contact'])) {
    return;
  }

  foreach ($tokenFunctions as $token) {
    if (array_key_exists($token, $tokens)) {
      $fn = $token . '_civitoken_get';
      foreach ($contactIDs as $contactID) {
        if (!isset($values[$contactID])) {
          $values[$contactID] = [];
        }
        $fn($contactID, $values[$contactID], $context, $job, $tokens[$token]);
      }
    }
  }
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
    $tokenFiles = _civitoken_civix_find_files($directory, '*.inc');
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
