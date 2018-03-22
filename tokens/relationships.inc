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
    $tokens[$token . '.display_name_' . $id] = $label . ' : Name of first contact found';
    $tokens[$token . '.first_name_' . $id] = $label . ' : First Name of first contact found';
    $tokens[$token . '.last_name_' . $id] = $label . ' : Last Name of first contact found';
    $tokens[$token . '.phone_' . $id] = $label . ' : Phone of first contact found';
    $tokens[$token . '.email_' . $id] = $label . ' : Email of first contact found';
    $tokens[$token . '.id_' . $id] = $label . ' : ID of first contact found';
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
  $result = civicrm_api3('relationship_type', 'get', array('is_active' => 1, 'options' => array('limit' => 0)));
  $relationships = array();
  foreach ($result['values'] as $key => $values) {
    $relationships[$key . '_a_b'] = 'Relationship: ' . $values['label_b_a'];
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
  $fields = array(
    'display_name',
    'phone',
    'email',
    'id',
    'first_name',
    'last_name',
  );
  $relationships = civicrm_api3('relationship', 'get', array(
    'contact_id_a' => $cid,
    'is_active' => TRUE,
    'api.contact.getsingle' => array(
      'id' => '$value.contact_id_b',
      'return' => $fields,
    ),
  ));
  foreach ($relationships['values'] as $relationship) {
    if (!empty($relationship['start_date']) && strtotime($relationship['start_date']) > strtotime('now')) {
      continue;
    }
    if (!empty($relationship['end_date']) && strtotime($relationship['start_date']) < strtotime('now')) {
      continue;
    }
    if (!empty($value['relationships.id_' . $relationship['relationship_type_id'] . '_a_b'])) {
      // ID is set - so a contact with this relationship has been found, skip.
      continue;
    }

    foreach ($fields as $key) {
      if (empty($relationship['api.contact.getsingle'][$key])) {
        $value['relationships.' . $key . '_' . $relationship['relationship_type_id'] . '_a_b'] = '';
      }
      else {
        $value['relationships.' . $key . '_' . $relationship['relationship_type_id'] . '_a_b'] = $relationship['api.contact.getsingle'][$key];
      }
    }
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
