<?php
function phones_civitoken_declare($token) {
  return array(
    $token . '.work_phone' => 'Work Phone',
    $token . '.mobiles' => 'Mobile Phone',
  );
}

/*
 * based on Michael McAndrew's code but updated to use v3 & to only query for
 * required fields
 * https://github.com/michaelmcandrew/civicrm-addressblocktoken/blob/master/addressblocktoken.module
 */
function phones_civitoken_get($cid, &$value) {
  $value['address.work_phone'] = phones_civitoken_getPhone($cid, $value, 'Work');
  $value['address.mobiles'] = phones_civitoken_getPhone($cid, $value, 'mobile');
  return $value;
}

/**
 * Function get additional specific phones
*/
function phones_civitoken_getPhone($contactID, $contactDetails, $type) {
  $params = array(
    'contact_id' => $contactID,
    'version' => 3,
    'sequential' => 1,
  );
  if($type == 'mobile'){
    $params['phone_type_id'] = 2;
  }
  else{
    $locationTypes = civicrm_api3('phone', 'getoptions', array(
      'field' => 'location_type',
    ));
    $params['location_type_id'] = array_search($type, $locationTypes['values']);
  }

  $phones = civicrm_api('phone', 'get', $params);
  if($phones['count']){
    return $phones['values'][0]['phone'];
  }
}
