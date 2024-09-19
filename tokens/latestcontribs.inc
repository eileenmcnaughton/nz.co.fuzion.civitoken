<?php

function latestcontribs_civitoken_declare($token) {
  return [
    $token . '.softcredit_name' => 'Latest Contrib Soft Credit Name',
    $token . '.total_amount' => 'Total amount of latest completed contribution',
    $token . '.receive_date' => 'Receive date of latest completed contribution',
    $token . '.financial_type' => 'Financial type of latest completed contribution',
  ];
}


/**
 * @param int $cid
 * @param $value
 *
 * @return array
 *
 * @throws \CRM_Core_Exception
 */
function latestcontribs_civitoken_get(int $cid, &$value): array {
  $lastCompletedContribution =  civicrm_api3('Contribution', 'get', [
    'sequential' => 1,
    'contact_id' => $cid,
    'is_test' => 0,
    'options' => ['sort' => 'receive_date DESC', 'limit' => 1],
  ])['values'][0] ?? NULL;
  $value['latestcontribs.softcredit_type'] = '';
  if ($lastCompletedContribution && isset($lastCompletedContribution['soft_credit'])) {
    $types = [];
    foreach ($lastCompletedContribution['soft_credit'] as $softCredit) {
      $types[] = $softCredit['soft_credit_type_label'];
    }
    $value['latestcontribs.softcredit_type']  = implode($types);
    $value['latestcontribs.softcredit_name'] = _latestcontribs_soft_credit_name($lastCompletedContribution['soft_credit']);
  }
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
 *
 * @return array
 * @throws \CRM_Core_Exception
 * @throws \Exception
 */
function _latestcontribs_get_last_completed_contribution($cid, $value) {
  $sql = 'SELECT civicrm_contribution.receive_date,
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
  ';
  $params[1] = [$cid, 'Integer'];
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
 * @param array $softCredits
 *
 * @return string
 * @throws \CRM_Core_Exception
 */
function _latestcontribs_soft_credit_name($softCredits) {

  $softcredit_formatted = "";

  $connector = '';
  $i = 0;
  $total = count($softCredits);
  foreach ($softCredits as $softcredit) {
    $i++;
    $softcredit_formatted .= $connector . $softcredit['contact_name'];
    if ($i === ($total - 1)) {
      $connector = ' and ';
    }
    else {
      $connector = ', ';
    }
  }
  return $softcredit_formatted;
}
