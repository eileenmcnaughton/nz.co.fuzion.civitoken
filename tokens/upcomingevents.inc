<?php

/**
 * Declare token for upcoming events.
 *
 * @param string $token
 *
 * @return array
 */
function upcomingevents_civitoken_declare($token) {
  return [
    $token . '.upcoming_events' => 'Upcoming Events',
    $token . '.upcoming_event_summaries' => 'Upcoming Events No Titles',
    $token . '.upcoming_titles' => 'Upcoming Event Titles',
    $token . '.upcoming_selection' => 'Upcoming Event Selection',
  ];
}

/**
 * Get upcoming events token.
 *
 * @param int $cid
 * @param array $value
 */
function upcomingevents_civitoken_get($cid, &$value) {
  $curDate = date('Y-m-d');
  $events = civicrm_api3('event', 'get', [
    'sequential' => 1,
    'start_date' => ['>=' => $curDate],
    'sort' => 'start_date',
    'options' => ['limit' => 1],
  ]);

  // Make sure there are events to return.
  if ($events['count'] == 0) {
    return;
  }
  $value['upcomingevents.upcoming_selection'] = $events['values'][0]['title'];
  $participants = civicrm_api3('participant', 'get', [
    'event_start_date_low' => 'now',
    'sort' => 'event_start_date',
    'contact_id' => $cid,
    'api.event.get' => [
      'return' => ['event_type_id', 'summary'],
      'start_date' => ['>=' => $curDate],
    ],
  ]);

  if (empty($participants['count'])) {
    return;
  }
  $upcomingEvents = "<table>";
  $upcomingEventSummaries = '';
  $upcomingEventTitles = [];

  foreach ($participants['values'] as $particpantID => $participant) {
    if (empty($participant['api.event.get']['values'][0]) || $participant['api.event.get']['values'][0] == 16) {
      continue;
    }
    $upcomingEventTitles[] = $participant['event_title'];
    $upcomingEvents .= "<tr><td><hr /></td></tr> <tr><td><h2>{$participant['event_title']}</h2></td></tr>"
      . "<tr><td>" . nl2br($participant['api.event.get']['values'][0]['summary']) . " </td></tr>
        ";
    $upcomingEventSummaries .= "<table><tr><td><hr /></td></tr><tr><td>" . nl2br($participant['api.event.get']['values'][0]['summary']) . "</td></tr></table>";
  }
  $upcomingEvents .= "</table>";
  $value['upcomingevents.upcoming_titles'] = implode(" and ", $upcomingEventTitles);
  $value['upcomingevents.upcoming_events'] = $upcomingEvents;
  $value['upcomingevents.upcoming_event_summaries'] = $upcomingEventSummaries;
}

/**
 * Probably should use civicrm date formatting but quick & dirty.
 *
 * @param string $date
 *
 * @return string
 */
function upcomingevents_date_format($date) {
  return date('l\s \a\t g a', strtotime($date));
}
