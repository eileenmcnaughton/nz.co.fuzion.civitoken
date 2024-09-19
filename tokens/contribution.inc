<?php

function contribution_civitoken_declare($token) {
  return [
    $token . '.total_this_year_contribution_amount' => 'Total amount given this year',
    $token . '.total_last_year_contribution_amount' => 'Total amount given last year',
    $token . '.total_contribution_amount' => 'Total contributed amount all time',
    $token . '.total_this_year_contribution_deductible_amount' => 'Total deductible amount given this year',
    $token . '.total_last_year_contribution_deductible_amount' => 'Total deductible amount given last year',
    $token . '.total_contribution_deductible_amount' => 'Total deductible contributed amount all time',
    $token . '.latest_recur_contribution_id' => 'Latest Recur Contribution ID',
  ];
}

/**
 * @param int $contactID
 * @param array $value
 *
 * @return mixed
 * @throws \CRM_Core_Exception
 */
function contribution_civitoken_get($contactID, &$value) {
  $value['contribution.total_this_year_contribution_amount'] = \CRM_Utils_Money::format(_contribution_civitoken_get_total_amount_donated_by_year($contactID, 0));
  $value['contribution.total_last_year_contribution_amount'] = \CRM_Utils_Money::format(_contribution_civitoken_get_total_amount_donated_by_year($contactID, -1));
  $value['contribution.total_contribution_amount'] = \CRM_Utils_Money::format(_contribution_civitoken_get_total_amount_donated($contactID));
  $value['contribution.total_this_year_contribution_deductible_amount'] = \CRM_Utils_Money::format(_contribution_civitoken_get_total_deductible_amount_donated_by_year($contactID, 0));
  $value['contribution.total_last_year_contribution_deductible_amount'] = \CRM_Utils_Money::format(_contribution_civitoken_get_total_deductible_amount_donated_by_year($contactID, -1));
  $value['contribution.total_contribution_deductible_amount'] = \CRM_Utils_Money::format(_contribution_civitoken_get_total_deductible_amount_donated($contactID));
  $value['contribution.latest_recur_contribution_id'] = _contribution_civitoken_get_latest_recur_contribution_id($contactID);
  return $value;
}

/**
 * Get latest recur contribution id for the contact.
 *
 * @param int $cid
 *   The contact id.
 *
 * @return int
 * @throws \CRM_Core_Exception
 */
function _contribution_civitoken_get_latest_recur_contribution_id($cid) {
  $recur = civicrm_api3('ContributionRecur', 'get', [
    'sequential' => 1,
    'contact_id' => $cid,
    'contribution_status_id' => "In Progress",
    'options' => ['sort' => "id DESC"],
  ]);
  if (!empty($recur['count']) && !empty($recur['values'][0]['id'])) {
    return $recur['values'][0]['id'];
  }
  return NULL;
}

/**
 * Get total donated all time.
 *
 * @param int $cid
 *   The contact id.
 *
 * @return float
 */
function _contribution_civitoken_get_total_amount_donated($cid) {
  $sql = "SELECT SUM(total_amount) as total_amount
    FROM civicrm_contribution
    WHERE is_test = 0
    AND contribution_status_id = 1
    AND contact_id = %1
    GROUP BY contact_id";
  $params[1] = [$cid, 'Integer'];
  return (float) CRM_Core_DAO::singleValueQuery($sql, $params);
}

/**
 * Get total deductible amount donated all time.
 *
 * @param int $cid
 *   The contact id.
 *
 * @return float
 */
function _contribution_civitoken_get_total_deductible_amount_donated($cid) {
  return (float) CRM_Core_DAO::singleValueQuery('
    SELECT (SUM(total_amount) - SUM(non_deductible_amount)) as total_amount
    FROM civicrm_contribution
    WHERE is_test = 0
    AND contribution_status_id = 1
    AND contact_id = %1
    GROUP BY contact_id
    ', [1 => [$cid, 'Integer']]
  );
}

/**
 * Get total donated in a year.
 * To get the total for current year. Use offset = 0. The get the total donated last year use offset = -1
 *
 * @param int $cid
 *   The contact id.
 * @param int $offset
 *   The offset for the year calculed by current year. For example last year is offset=-1.
 *
 * @return float
 */
function _contribution_civitoken_get_total_amount_donated_by_year($cid, $offset) {
  $sql = 'SELECT SUM(total_amount) as total_amount
    FROM civicrm_contribution
    WHERE is_test = 0
    AND contribution_status_id = 1
    AND contact_id = %1
    AND YEAR(receive_date) = (YEAR(now()) + %2)
    GROUP BY contact_id';
  $params[1] = [$cid, 'Integer'];
  $params[2] = [$offset, 'Integer'];
  return (float) CRM_Core_DAO::singleValueQuery($sql, $params);
}

/**
 * Get total decuctible amount donated in a year.
 * To get the total for current year. Use offset = 0. The get the total donated last year use offset = -1
 *
 * @param int $cid
 *   The contact id.
 * @param int $offset
 *   The offset for the year calculed by current year. For example last year is offset=-1.
 *
 * @return float
 */
function _contribution_civitoken_get_total_deductible_amount_donated_by_year($cid, $offset) {
  $sql = "SELECT (SUM(total_amount) - SUM(non_deductible_amount)) as total_amount
   FROM civicrm_contribution
    WHERE is_test = 0
    AND contribution_status_id = 1
    AND contact_id = %1
    AND YEAR(receive_date) = (YEAR(now()) + %2)
    GROUP BY contact_id";
  $params[1] = [$cid, 'Integer'];
  $params[2] = [$offset, 'Integer'];
  return (float) CRM_Core_DAO::singleValueQuery($sql, $params);
}
