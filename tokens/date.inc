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
function date_civitoken_declare($token){
  return array(
    $token. '.today_format_full' => 'Today\'s Date',
  );
}

/**
 * @param $cid
 * @param $value
 */
function date_civitoken_get($cid, &$value){
  $value[$cid]['date.today_format_full'] = CRM_Utils_Date::customFormat(date('Y-m-d'), NULL, array('Y', 'm', 'd'));
}
