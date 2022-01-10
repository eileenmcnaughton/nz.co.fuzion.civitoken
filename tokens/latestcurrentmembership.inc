<?php

function latestcurrentmembership_civitoken_declare($token) {
  return [
    $token . '.type' => 'Latest Current Membership Type',
    $token . '.end_date' => 'Latest Current Membership End Date',
    $token . '.end_date_raw' => 'Latest Current Membership End Date (raw)',
    $token . '.start_date' => 'Latest Current Membership Start Date',
    $token . '.start_date_raw' => 'Latest Current Membership Start Date (raw)',
    $token . '.join_date' => 'Latest Current Membership Join Date',
    $token . '.join_date_raw' => 'Latest Current Membership Join Date (raw)',
    $token . '.fee' => 'Latest Current Membership Fee',
    $token . '.status' => 'Latest Current Membership Status',
    $token . '.alltype' => 'Latest Membership Type',
    $token . '.allend_date' => 'Latest Membership End Date',
    $token . '.allend_date_raw' => 'Latest Membership End Date (raw)',
    $token . '.allstart_date' => 'Latest Membership Start Date',
    $token . '.allstart_date_raw' => 'Latest Membership Start Date (raw)',
    $token . '.alljoin_date' => 'Latest Membership Join Date',
    $token . '.alljoin_date_raw' => 'Latest Membership Join Date (raw)',
    $token . '.allfee' => 'Latest Membership Fee',
    $token . '.allstatus' => 'Latest Membership Status',
    $token . '.membership_id' => 'Latest Membership ID',
  ];
}


function latestcurrentmembership_civitoken_get($cid, &$value) {
  latestcurrentmembership_get_tokens($cid, $value, FALSE);
  latestcurrentmembership_get_tokens($cid, $value, TRUE);
}

function latestcurrentmembership_get_tokens($cid, &$value, $onlyActive) {
  $params = [
    'version' => 3,
    'contact_id' => $cid,
    'sequential' => 1,
    'api.membership_type.getsingle' => 1,
    'is_test' => 0,
    'options' => ['limit' => 1, 'sort' => 'end_date DESC'],
    //'return' => array('membership_end_date', 'end_date', 'start_date', 'status_id', 'minimum_fee', 'membership_name', 'id', 'membership_type_id'),
  ];
  $prefix = 'all';
  if ($onlyActive) {
    $params['is_active'] = 1;
    $params['active_only'] = 1;
    $prefix = '';
  }

  static $statuses;
  if (empty($statuses)) {
    $statuses = civicrm_api3('membership', 'getoptions', ['field' => 'status_id']);
    $statuses = $statuses['values'];
  }
  try {
    $membership = civicrm_api3('membership', 'getsingle', $params);
  }
  catch (Exception $e) {
    $tokens = ['type', 'end_date', 'start_date', 'join_date', 'fee', 'status'];
    foreach ($tokens as $token) {
      $value['latestcurrentmembership.' . $prefix . $token] = NULL;
    }
    return $value;
  }

  if (!CRM_Utils_Array::value('is_error', $membership)) {
    if (isset($prefix)) {
      if (isset($membership['api.membership_type.getsingle']['name'])) {
        $value['latestcurrentmembership.' . $prefix . 'type'] = $membership['api.membership_type.getsingle']['name'];
      }
      if (isset($membership['end_date'])) {
        $value['latestcurrentmembership.' . $prefix . 'end_date'] = !empty($membership['end_date']) ? CRM_Utils_Date::customFormat($membership['end_date']) : '';
        $value['latestcurrentmembership.' . $prefix . 'end_date_raw'] = !empty($membership['end_date']) ? CRM_Utils_Date::customFormat($membership['end_date'], "%Y%m%d") : '';
      }
      if (isset($membership['start_date'])) {
        $value['latestcurrentmembership.' . $prefix . 'start_date'] = CRM_Utils_Date::customFormat($membership['start_date']);
        $value['latestcurrentmembership.' . $prefix . 'start_date_raw'] = CRM_Utils_Date::customFormat($membership['start_date'], "%Y%m%d");
      }
      if (isset($membership['join_date'])) {
        $value['latestcurrentmembership.' . $prefix . 'join_date'] = CRM_Utils_Date::customFormat($membership['join_date']);
        $value['latestcurrentmembership.' . $prefix . 'join_date_raw'] = CRM_Utils_Date::customFormat($membership['join_date'], "%Y%m%d");
      }
      if (isset($membership['api.membership_type.getsingle']['minimum_fee'])) {
        $value['latestcurrentmembership.' . $prefix . 'fee'] = CRM_Utils_Money::format($membership['api.membership_type.getsingle']['minimum_fee']);
      }
      if (isset($statuses[$membership['status_id']])) {
        $value['latestcurrentmembership.' . $prefix . 'status'] = $statuses[$membership['status_id']];
      }
      if (isset($membership['id'])) {
        $value['latestcurrentmembership.' . $prefix . 'membership_id'] = $membership['id'];
      }
    }
  }
  return $value;
}
