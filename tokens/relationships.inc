<?php
/**
 * @file
 * Relationship tokens.
 */

/**
 * Declare tokens this file offers.
 *
 * @param array $token
 *   Array of tokens to add to.
 *
 * @return array
 *   Available tokens.
 */
function relationships_civitoken_declare($token) {
  $tokens = array();
  foreach (relationships_get_relationship_list() as $id => $label) {
    $tokens[$token . '.display_name_' . $id] = $label;
  }
  return $tokens;
}

/**
 * Get list of enabled relationship types.
 *
 * @return array
 *   relationships keyed by type and direction (A-B direction only at this
 *   stage).
 *
 * @throws \CiviCRM_API3_Exception
 */
function relationships_get_relationship_list() {
  $result = civicrm_api3('relationship_type', 'get', array('is_active' => 1));
  $relationships = array();
  foreach ($result['values'] as $key => $values) {
    $relationships[$key . '_a_b'] = 'Relationship: ' . $values['label_b_a'] . ' : Name of first contact found';
  }
  return $relationships;
}

/**
 * Get Relationship Tokens.
 *
 * @param int $cid
 *   contact ID
 * @param array $value
 *   contact values
 *
 * @return array
 *   array of token values for the contact.
 * @throws \CiviCRM_API3_Exception
 */
function relationships_civitoken_get($cid, &$value) {
  $relationships = civicrm_api3('relationship', 'get', array(
    'contact_id_a' => $cid,
    'is_active' => TRUE,
    'api.contact.getvalue' => array(
      'id' => '$value.contact_id_b',
      'return' => 'display_name',),
    )
  );
  foreach ($relationships['values'] as $relationship) {
    if (!empty($relationship['start_date']) && strtotime($relationship['start_date']) > strtotime('now')) {
      continue;
    }
    if (!empty($relationship['end_date']) && strtotime($relationship['start_date']) < strtotime('now')) {
      continue;
    }
    $value['relationships.display_name_' . $relationship['relationship_type_id'] . '_a_b'] = $relationship['api.contact.getvalue'];
  }
  return $value;
}

/**
 * Get the relationship type ID from the given string.
 *
 * @param string $string
 *   String to parse for relationship type.
 *
 * @return int
 *   relationship type
 */
function relationships_civitoken_get_relationship_id($string) {
  $matches = array();
  preg_match("/(.*)name_(.*)_(.*)_(.*)/", $string, $matches);
  return isset($matches[2]) ? $matches[2] : 0;
}
