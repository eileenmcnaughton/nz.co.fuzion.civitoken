<?php
/**
 * @param $token
 * @return array
 */
function relationships_civitoken_declare($token) {
  $tokens = array();
  foreach (relationships_get_relationship_list() as $id => $label) {
    $tokens[$token . '.display_name_' . $id] = $label;
  }
  return $tokens;
}

function relationships_get_relationship_list() {
  $result = civicrm_api3('relationship_type', 'get', array());
  $relationships = array();
  foreach ($result['values'] as $key => $values) {
    $relationships[$key . '_a_b'] = 'Relationship: ' . $values['label_b_a'] . ' : Name of first contact found';
  }
  return $relationships;
}

/**
 * @param int $cid contact ID
 * @param array $value contact values
 *
 * @return mixed
 */
function relationships_civitoken_get($cid, &$value, $context) {
  $relationships = civicrm_api3('relationship', 'get', array(
    'contact_id_a' => $cid,
    'is_active' => TRUE,
    'api.contact.getvalue' => array('id' => '$value.contact_id_b', 'return' => 'display_name',),
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
 * @param $string
 * @return string
 */
function relationships_civitoken_get_relationship_id($string) {
  $matches = array();
  preg_match("/(.*)name_(.*)_(.*)_(.*)/", $string, $matches);
  return isset($matches[2]) ?$matches[2] : '';
}
