<?php
/**
 * Created by PhpStorm.
 * User: eileen
 * Date: 18/12/2014
 * Time: 12:14 PM
 */

/**
 * @param $token
 * @return array
 */
function user_civitoken_declare($token){
  return array(
    $token. '.name' => 'Username',
    $token. '.id' => 'UserID',
  );
}

/**
 * @param $cid
 * @param $value
 */
function user_civitoken_get($cid, &$value){
  try {
    $user = civicrm_api3('user', 'getsingle', array('contact_id' => $cid));
    $value['user.name'] = $user['name'];
    $value['user.id'] = $user['id'];
  }
  catch (CiviCRM_API3_Exception $e) {
    $value['user.name'] = '';
    $value['user.id'] = '';
  }


}
