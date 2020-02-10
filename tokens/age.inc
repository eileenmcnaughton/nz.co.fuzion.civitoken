<?php

function age_civitoken_declare($token) {
  return [
    $token . '.years' => 'Age in Years',
    $token . '.months' => 'Age in Months',
    $token . '.units' => 'Age Units',
  ];
}

/**
 * Get Age tokens
 *
 * @param int $cid
 * @param array $value
 *
 * @return mixed
 */
function age_civitoken_get($cid, &$value) {
  $value['age.years'] = $value['age.months'] = $value['age.units'] = NULL;
  $birthDate = CRM_Utils_Array::value('birth_date', $value, "not set");
  if ($birthDate == "not set") {
    $birthDate = civicrm_api3('Contact', 'getvalue', [
      'id' => $cid,
      'return' => "birth_date",
    ]);
  }
  if (empty($birthDate)) {
    return $value;
  }
  $age = CRM_Utils_Date::calculateAge($birthDate);
  if (!empty($age['years'])) {
    $value['age.years'] = $age['years'];
    $value['age.units'] = ts('years');
  }
  else {
    $value['age.months'] = $age['months'];
    $value['age.units'] = ts('months');
  }
  return $value;
}
