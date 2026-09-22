<?php
/**
 * Created by IntelliJ IDEA.
 * User: emcnaughton
 * Date: 1/10/18
 * Time: 4:30 PM
 */
 return ['civitoken_enabled_tokens' => [
  'group_name' => 'Civitoken Settings',
  'group' => 'civitoken',
  'name' => 'civitoken_enabled_tokens',
  'type' => 'String',
  'is_domain' => 1,
  'is_contact' => 0,
  'description' => 'Enabled tokens',
  'title' => 'Enabled tokens',
  'help_text' => '',
  'html_type' => 'Checkboxes',
  'quick_form_type' => 'Checkboxes',
  'pseudoconstant' => [
     'callback' => 'civitoken_get_flattened_list_all',
   ],
]];
