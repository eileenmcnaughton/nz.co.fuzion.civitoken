<?php

/**
 * @param array $token
 *
 * @return array
 */
function nextpendingcontribution_civitoken_declare($token) {
  return [
    $token . '.softcredit_name' => 'Soft Credit Name(s) of next pending contribution',
    $token . '.total_amount' => 'Total amount of next pending contribution',
    $token . '.receive_date' => 'Receive date of next pending contribution',
    $token . '.financial_type' => 'Financial type of next pending contribution',
    $token . '.balance_due' => 'Balance due of the next pending contribution',
  ];
}

/**
 * @param int $cid
 * @param array $value
 *
 * @return array
 * @throws \CRM_Core_Exception
 */
function nextpendingcontribution_civitoken_get($cid, &$value): array {
  $value['nextpendingcontribution.softcredit_name'] = _nextpendingcontribution_soft_credit_name($cid);
  $value = _nextpendingcontribution_get_next_pending_contribution($cid, $value);

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
 */
function _nextpendingcontribution_get_next_pending_contribution($cid, $value) {
  $value['nextpendingcontribution.total_amount'] = '';
  $value['nextpendingcontribution.receive_date'] = '';
  $value['nextpendingcontribution.financial_type'] = '';

  try {
    $pending_contribution_status = civicrm_api3('OptionValue', 'getvalue', ['option_group_id' => 'contribution_status', 'return' => 'value', 'name' => 'Pending']);
    $sql = "SELECT
      civicrm_contribution.id,
      civicrm_contribution.receive_date,
      civicrm_contribution.total_amount,
      civicrm_financial_type.name as financial_type
      FROM civicrm_contribution
      LEFT JOIN civicrm_financial_type ON civicrm_contribution.financial_type_id = civicrm_financial_type.id
      WHERE
      civicrm_contribution.is_test = 0
      AND civicrm_contribution.contribution_status_id = %2
      AND civicrm_contribution.contact_id = %1
      ORDER BY civicrm_contribution.receive_date ASC
      LIMIT 0,1
          ";
    $params[1] = [$cid, 'Integer'];
    $params[2] = [$pending_contribution_status, 'Integer'];

    $dao = CRM_Core_DAO::executeQuery($sql, $params);
    if ($dao->fetch()) {
      $value['nextpendingcontribution.total_amount'] = \CRM_Utils_Money::format($dao->total_amount);
      $receive_date = new DateTime($dao->receive_date);
      $value['nextpendingcontribution.receive_date'] = \CRM_Utils_Date::customFormat($receive_date->format('Y-m-d'));
      $value['nextpendingcontribution.financial_type'] = $dao->financial_type;

      $balanceDue = CRM_Core_BAO_FinancialTrxn::getPartialPaymentWithType($dao->id, 'contribution', FALSE, $dao->total_amount);
      $value['nextpendingcontribution.balance_due'] = \CRM_Utils_Money::format($balanceDue);
    }
  }
  catch (Exception $e) {
    // do nothing
  }
  return $value;
}

/**
 * Get the name of the soft credits. Returns an empty string when no soft credit are found.
 *
 * @param int $cid
 *   The contact id.
 *
 * @return string
 * @throws \CRM_Core_Exception
 */
function _nextpendingcontribution_soft_credit_name($cid): string {
  $params = [
    'sequential' => 1,
    'contact_id' => $cid,
    'is_test' => 0,
    'contribution_status_id' => 'Pending',
    'options' => ['sort' => "receive_date DESC", 'limit' => 1],
  ];
  $softcredit_formatted = "";

  $softcredit_name = civicrm_api3('Contribution', 'get', $params)['values'][0] ?? NULL;

  if ($softcredit_name && isset($softcredit_name['soft_credit'])) {
    $connector = "";
    $i = 0;
    $total = count($softcredit_name['soft_credit']);
    foreach ($softcredit_name['soft_credit'] as $softcredit) {
      $i++;
      $softcredit_formatted .= $connector . $softcredit['contact_name'];
      if ($i === ($total - 1)) {
        $connector = " and ";
      }
      else {
        $connector = ", ";
      }
    }
  }
  return $softcredit_formatted;
}
