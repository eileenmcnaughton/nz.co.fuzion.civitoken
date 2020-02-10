<?php
function phones_civitoken_declare($token) {
  return [
    $token . '.work_phone' => 'Work Phone',
    $token . '.mobiles' => 'Mobile Phone',
  ];
}

/**
 * Get Phone tokens
 *
 * @param int $cid
 * @param array $value
 *
 * @return mixed
 */
function phones_civitoken_get($cid, &$value) {
  $value['phones.work_phone'] = phones_civitoken_getPhone($cid, $value, 'Work');
  $value['phones.mobiles'] = phones_civitoken_getPhone($cid, $value, 'mobile');
  return $value;
}

/**
 * Function get additional specific phones
 */
function phones_civitoken_getPhone($contactID, $contactDetails, $type) {
  $params = [
    'contact_id' => $contactID,
    'version' => 3,
    'sequential' => 1,
  ];
  if ($type === 'mobile') {
    $params['phone_type_id'] = 2;
  }
  else {
    $locationTypes = civicrm_api3('phone', 'getoptions', [
      'field' => 'location_type',
    ]);
    $params['location_type_id'] = array_search($type, $locationTypes['values']);
  }

  $phones = civicrm_api('phone', 'get', $params);
  if ($phones['count']) {
    return $phones['values'][0]['phone'];
  }
}
