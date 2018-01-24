<?php

function latestcontribs_civitoken_declare($token){
  return array(
    $token . '.softcredit_name' => 'Latest Contrib Soft Credit Name',
    $token . '.total_amount' => 'Total amount of latest completed contribution',
    $token . '.receive_date' => 'Receive date of latest completed contribution',
    $token . '.financial_type' => 'Financial type of latest completed contribution',
  );
}


function latestcontribs_civitoken_get($cid, &$value, $context){
  $value['latestcontribs.softcredit_name'] = _latestcontribs_soft_credit_name($cid);
	$value = _latestcontribs_get_last_completed_contribution($cid, $value);
	
  return $value;
}

/**
 * Gets the token information on the latest completed contribution from the database.
 * 
 * @param int $cid
 *   The contact id.
 * @param array $value
 *   The current value for the tokens.
 * @return array
 */
function _latestcontribs_get_last_completed_contribution($cid, $value) {
	$sql = "SELECT civicrm_contribution.receive_date, 
				civicrm_contribution.total_amount, 
				civicrm_financial_type.name as financial_type
				FROM civicrm_contribution 
				LEFT JOIN civicrm_financial_type ON civicrm_contribution.financial_type_id = civicrm_financial_type.id
				WHERE 
				civicrm_contribution.is_test = 0 
				AND civicrm_contribution.contribution_status_id = 1
				AND civicrm_contribution.contact_id = %1
				ORDER BY civicrm_contribution.receive_date DESC
				LIMIT 0,1
				";
	$params[1] = array($cid, 'Integer');
	$value['latestcontribs.total_amount'] = '';
	$value['latestcontribs.receive_date'] = '';
	$value['latestcontribs.financial_type'] = '';
	$dao = CRM_Core_DAO::executeQuery($sql, $params);
	if ($dao->fetch()) {
		$value['latestcontribs.total_amount'] = \CRM_Utils_Money::format($dao->total_amount);
		$receive_date = new DateTime($dao->receive_date);
		$value['latestcontribs.receive_date'] = \CRM_Utils_Date::customFormat($receive_date->format('Y-m-d'));
		$value['latestcontribs.financial_type'] = $dao->financial_type;	
	}
	return $value;
}

/**
 * Get the name of the soft credits. Returns an empty string when no soft credit are found.
 * 
 * @param int $cid
 *   The contact id.
 * @return string
 */
function _latestcontribs_soft_credit_name($cid) {
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
	return $softcredit_formatted;
}
