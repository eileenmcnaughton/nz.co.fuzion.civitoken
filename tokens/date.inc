<?php
/**
 * Created by PhpStorm.
 * User: eileen
 * Date: 18/12/2014
 * Time: 12:14 PM
 */

/**
 * @param $token
 *
 * @return array
 */
function date_civitoken_declare($token) {
  return [
    $token . '.today_format_full' => 'Today\'s Date',
    $token . '.today_format_raw' => 'Today\'s Date (raw)',
    $token . '.today_format_year' => 'Today\'s Year',
  ];
}

/**
 * @param $cid
 * @param $value
 */
function date_civitoken_get($cid, &$value) {
  $value['date.today_format_full'] = CRM_Utils_Date::customFormat(date('Y-m-d'), NULL, ['Y', 'm', 'd']);
  $value['date.today_format_raw'] = date('Ymd');
  $value['date.today_format_year'] = date('Y');
}
