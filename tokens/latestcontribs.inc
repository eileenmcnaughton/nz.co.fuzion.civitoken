<?php

function latestcontribs_civitoken_declare($token){
  return array(
    $token . '.softcredit_name' => 'Latest Contrib Soft Credit Name',
  );
}


function latestcontribs_civitoken_get($cid, &$value, $context){
  $params = array(
    'sequential' => 1,
    'contact_id' => $cid,
    'is_test' => 0,
    'options' => array('sort' => "receive_date DESC", 'limit' => 1),
  );
  $softcredit_formatted = "";

  $softcredit_name = civicrm_api3('Contribution', 'getsingle', $params);

  if (isset($softcredit_name['soft_credit'])) {
    $connector = "";
    $i = 0;
    $total = count($softcredit_name['soft_credit']);
    foreach($softcredit_name['soft_credit'] as $softcredit) {
      $i++;
      $softcredit_formatted .= $connector.$softcredit['contact_name'];
      if ($i == ($total-1) ) {
        $connector = " and ";
      } else {
        $connector = ", ";
      }
    }
  }
  $value['latestcontribs.softcredit_name'] = $softcredit_formatted;
  return $value;
}
