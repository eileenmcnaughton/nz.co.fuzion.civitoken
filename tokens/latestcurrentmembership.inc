<?php

function latestcurrentmembership_civitoken_declare($token){
  return array(
    $token. '.type' => 'Latest Current Membership Type',
    $token. '.end_date' => 'Latest Current Membership End Date',
    $token. '.start_date' => 'Latest Current Membership Start Date',
    $token. '.fee' => 'Latest Current Membership Fee',
    $token. '.status' => 'Latest Current Membership Status',
    $token. '.alltype' => 'Latest Membership Type',
    $token. '.allend_date' => 'Latest Membership End Date',
    $token. '.allstart_date' => 'Latest Membership Start Date',
    $token. '.allfee' => 'Latest Membership Fee',
    $token. '.allstatus' => 'Latest Membership Status',
    );
}


function latestcurrentmembership_civitoken_get($cid, &$value, $context){
  latestcurrentmembership_get_tokens($cid, $value, FALSE);
  latestcurrentmembership_get_tokens($cid, $value, TRUE);
}

function latestcurrentmembership_get_tokens($cid, &$value, $onlyActive) {
  $params = array(
    'version' => 3,
    'contact_id' => $cid,
    'sequential' => 1,
    'api.membership_type.getsingle' => 1,
    'options' => array('limit' => 1, 'sort' => 'end_date DESC')
  );
  $prefix = 'all';
  if ($onlyActive) {
    $params['is_active'] = 1;
    $params['active_only'] = 1;
    $prefix = '';
  }

  static $statuses;
  if (empty($statuses)) {
    $statuses = civicrm_api3('membership', 'getoptions', array('field' => 'status_id'));
    $statuses = $statuses['values'];
  }
  $membership = civicrm_api('membership', 'getsingle', $params);

  if(!CRM_Utils_Array::value('is_error',$membership)){
    $value['latestcurrentmembership.' . $prefix . 'type'] = $membership['membership_name'];
    $value['latestcurrentmembership.' . $prefix . 'end_date']= date('d-m-Y', strtotime($membership['end_date']));
    $value['latestcurrentmembership.' . $prefix . 'start_date']= date('d-m-Y', strtotime($membership['start_date']));
    $value['latestcurrentmembership.' . $prefix . 'fee']= $membership['api.membership_type.getsingle']['minimum_fee'];
    $value['latestcurrentmembership.' . $prefix . 'status'] = $statuses[$membership['status_id']];
  }
  return $value;
}
