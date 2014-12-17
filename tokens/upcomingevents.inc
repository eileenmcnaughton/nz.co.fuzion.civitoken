<?php

function upcomingevents_civitoken_declare($token){
  return array(
    $token. '.upcoming_events' => 'Upcoming Events',
    $token. '.upcoming_event_summaries' => 'Upcoming Events No Titles',
    $token. '.upcoming_titles' => 'Upcoming Event Titles',
    $token. '.upcoming_selection' => 'Upcoming Event Selection',
   );
}

/*
 *
*/
function upcomingevents_civitoken_get($cid, &$value){
  $events = civicrm_api('event', 'get', array('sequential' => 1,'version' => 3, 'event_type_id' => 16, 'filters' => array('start_date_low' => 'now()'), 'debug' => 1, 'sort' => 'start_date', 'options' => array('limit' => 1)));
  $value['upcomingevents.upcoming_selection'] = $events['values'][0]['title'];
  $participants = civicrm_api('participant', 'get', array('version' => 3, 'event_start_date_low' => 'now', 'sort'=> 'event_start_date', 'contact_id' => $cid, 'api.event.get' => array('return' => array('event_type_id', 'summary'))));

  if(empty($participants['count'])){
    return;
  }
  $upcomingEvents = "<table>";

  foreach ($participants['values'] as $particpantID => $participant){
    if($participant['api.event.get']['values'][0] ==16){
      continue;
    }
    $upcomingEventTitles[]= $participant['event_title'];
    $upcomingEvents .="<tr><td><hr /></td></tr> <tr><td><h2>{$participant['event_title']}</h2></td></tr>"
      . "<tr><td>" . nl2br($participant['api.event.get']['values'][0]['summary']) . " </td></tr>
        ";
    $upcomingEventSummaries .= "<table><tr><td><hr /></td></tr><tr><td>" . nl2br($participant['api.event.get']['values'][0]['summary']) . "</td></tr></table>";

;
  }
  $upcomingEvents .= "</table>";
  $value['upcomingevents.upcoming_titles'] = implode(" and ",$upcomingEventTitles);
  $value['upcomingevents.upcoming_events'] = $upcomingEvents;
  $value['upcomingevents.upcoming_event_summaries'] = $upcomingEventSummaries;

  return;
}
/*
 * Probably should use civicrm date formatting but quick & dirty
 */
function upcomingevents_date_format($date){
  return date('l\s \a\t g a', strtotime($date));
}
