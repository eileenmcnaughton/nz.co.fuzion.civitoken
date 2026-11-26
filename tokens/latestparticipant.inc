<?php
return;

function latestparticipant_civitoken_declare($token) {
  return [
    $token . '.event_title' => 'Latest Participant Event Title',
    $token . '.event_start_date' => 'Latest Participant Event Start Date',
    $token . '.event_end_date' => 'Latest Participant Event End Date',
    $token . '.event_location' => 'Latest Participant Event Location',
    $token . '.event_summary' => 'Latest Participant Event Summary',
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
function latestparticipant_civitoken_get(int $cid, &$value): array {
  $value = _latestparticipant_get_last_participant($cid, $value);

  return $value;
}

/**
 * Gets the token information on the latest participant from the database.
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
function _latestparticipant_get_last_participant($cid, $value) {
  $sql = 'SELECT civicrm_event.start_date AS event_start_date,
    civicrm_event.end_date AS event_end_date,
    civicrm_event.title AS event_title,
    civicrm_event.summary AS event_summary,
    civicrm_address.street_address AS event_location
    FROM civicrm_event
    INNER JOIN civicrm_participant ON civicrm_participant.event_id = civicrm_event.id
    LEFT JOIN civicrm_loc_block ON civicrm_event.loc_block_id = civicrm_loc_block.id
    LEFT JOIN civicrm_address ON civicrm_address.id = civicrm_loc_block.address_id
    WHERE
    civicrm_participant.is_test = 0
--    AND civicrm_participant.status_id = 1
    AND civicrm_participant.contact_id = %1
    ORDER BY civicrm_participant.register_date DESC
    LIMIT 0,1
  ';
  $params[1] = [$cid, 'Integer'];
  $value['latestparticipant.event_title'] = '';
  $value['latestparticipant.event_start_date'] = '';
  $value['latestparticipant.event_end_date'] = '';
  $value['latestparticipant.event_location'] = '';
  $value['latestparticipant.event_summary'] = '';
  $dao = CRM_Core_DAO::executeQuery($sql, $params);
  if ($dao->fetch()) {
    $value['latestparticipant.event_title'] = $dao->event_title;
    $value['latestparticipant.event_start_date'] = $dao->event_start_date;
    $value['latestparticipant.event_end_date'] = $dao->event_end_date;
    $value['latestparticipant.event_location'] = $dao->event_location;
    $value['latestparticipant.event_summary'] = $dao->event_summary;
  }
  return $value;
}
