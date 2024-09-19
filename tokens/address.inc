<?php

function address_civitoken_declare($token) {
  return [
    $token . '.address_block' => 'Address Block',
    $token . '.address_block_text' => 'Address Block (Text)',
    $token . '.address_conditional_country' => 'Address Conditional Country',
  ];
}

/**
 * based on Michael McAndrew's code but updated to use v3 & to only query for
 * required fields
 * https://github.com/michaelmcandrew/civicrm-addressblocktoken/blob/master/addressblocktoken.module
 *
 * @throws \CRM_Core_Exception
 */
function address_civitoken_get($cid, &$value) {
  $contact = getMissingAddressFields($cid, $value);
  $domain = civicrm_api3('domain', 'getsingle', ['current_domain' => 1]);
  if (!empty($contact['country_id'])) {
    $format = CRM_Core_DAO::singleValueQuery("SELECT format FROM civicrm_address_format a LEFT JOIN civicrm_country c ON c.address_format_id = a.id WHERE c.id = {$contact['country_id']}");
    // Call getMissingAddressFields again, you may have different address fields now.
    $contact = getMissingAddressFields($cid, $value, $format);
  }

  if (empty($format)) {
    $format = \Civi::settings()->get('mailing_format');
  }

  if (isset($domain['domain_address']) && $contact['country_id'] == $domain['domain_address']['country_id']) {
    //without unsetting this it seems to give unexpected results
    unset($contact['country_id'], $contact['country']);
  }
  else {
    $value['address.address_conditional_country'] = $contact['country'] = $contact['address.address_conditional_country'] = strtoupper($contact['country']);
  }

  $format = str_replace('{address.address_conditional_country}', CRM_Utils_Array::value('country', $contact), $format);
  $value['address.address_block_text'] = CRM_Utils_Address::format($contact, $format);
  $value['address.address_block'] = nl2br(CRM_Utils_Address::format($contact, $format));
  return $value;
}

/**
 * Function will gather any addition address info required for the mailing label. ie. it
 * will find out the fields required & if there are any for which the key is not currently
 * set it will look them up via api
 * if keys are set but empty this is assumed to reflect underlying info & not looked up
 * note assume that setting is cached in underlying function & doesn't require caching here
 * to prevent multipl calls on the DB
 *
 * @param int $contactID  relevant contact id
 * @param array $contactDetails prepopulated values array for given contact
 * @param string $format The desired address format.
 *
 */
function getMissingAddressFields($contactID, $contactDetails, $format = '') {
  if (empty($format)) {
    $format = \Civi::settings()->get('mailing_format');
  }
  // country & display name are used even if their token is not - e.g organisation name
  $requiredFields = array_merge(_extractTokens($format), ['country', 'display_name']);

  if (in_array('addressee', $requiredFields)) {
    static $addresseeFieldString;
    if (empty($addresseeFieldString)) {

      $optionValues = civicrm_api3('option_value', 'get', ['option_group_name' => 'addressee']);
      foreach ($optionValues['values'] as $optionValue) {
        $addresseeFieldString .= $optionValue['name'];
      }
      $addresseeFields = _extractTokens($addresseeFieldString);
      $requiredFields = array_merge($requiredFields, $addresseeFields);
    }
  }
  $customMappings = [
    'individual_prefix' => 'prefix_id',// abnormal handling in api/bao layer
    'addressee' => 'addressee_display',
    'state_province_name' => 'state_province',
  ];
  $fieldsToFill = [];
  foreach ($requiredFields as $requiredField) {
    if (empty($contactDetails[$requiredField])) {
      $fieldsToFill[] = $requiredField;
    }
  }

  foreach ($customMappings as $origField => $substituteField) {
    if (in_array($origField, $fieldsToFill, TRUE)) {
      //our anomalies :-( - not sure prefix works as a return on api
      $fieldsToFill[] = $substituteField;
    }
  }

  // check if we need to retrieve fields to fill this token
  if (empty($fieldsToFill)) {
    // there is no more info to be retrieved
    return $contactDetails;
  }

  $contact = civicrm_api('contact', 'getsingle', ['version' => 3, 'id' => $contactID, 'return' => $fieldsToFill]);
  if (in_array('addressee', $requiredFields)) {
    $addresseeFields = _extractTokens(CRM_Utils_Array::value('addressee', $contact));
    $notFilled = array_diff($addresseeFields, $fieldsToFill);
    if (!empty($notFilled)) {
      $contact = array_merge($contact, civicrm_api('contact', 'getsingle', ['version' => 3, 'id' => $contactID, 'return' => $notFilled]));
    }
  }
  foreach ($customMappings as $origField => $substituteField) {
    if (!empty($contact[$substituteField]) && empty($contact[$origField])) {
      // individual prefix is an anomaly :-(
      $contact[$origField] = $contact[$substituteField];
    }
  }
  return array_merge($contact, $contactDetails);
}

/**
 * regex the (contact) tokens out of a string
 *
 * @param string $string
 *
 * @return string
 */
function _extractTokens($string) {
  $tokens = [];
  preg_match_all('/(?<!\{|\\\\)\{contact.(\w+)\}(?!\})/',
    $string,
    $tokens,
    PREG_PATTERN_ORDER
  );
  return $tokens[1];
}
