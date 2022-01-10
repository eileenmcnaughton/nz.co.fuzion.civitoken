<?php

namespace Civi\Token;

use Civi\Token\Event\TokenRegisterEvent;
use Civi\Token\Event\TokenValueEvent;
use Civi\Token\TokenProcessor;
use Civi\Token\TokenRow;
use Symfony\Component\EventDispatcher\EventSubscriberInterface;

/**
 * Class to declare out CiviTokens to the TokenProcessor.
 *
 * We implement the EventSubscriberInterface because it seems to help us not
 * have to make the functions static.
 */
class CiviTokens implements EventSubscriberInterface{

  /**
   * Get the implemented functions.
   * @return array
   */
  public static function getSubscribedEvents(): array {
    return [
      'civi.token.list' => 'registerTokens',
      'civi.token.eval' => 'evaluateTokens',
    ];
  }

  /**
   * Register the declared tokens.
   *
   * @param \Civi\Token\Event\TokenRegisterEvent $registerEvent
   *   The registration event. Add new tokens using register().
   *
   * @throws \CiviCRM_API3_Exception
   */
  public function registerTokens(TokenRegisterEvent $registerEvent): void {
    if (!$this->checkActive($registerEvent->getTokenProcessor())) {
      return;
    }
    foreach ($this->getTokenMetadata() as $entity => $field) {
      foreach ($field as $fieldKey => $label) {
        [, $tokenName] = explode('.', $fieldKey);
        $registerEvent->register([
          'entity' => $entity,
          'field' => $tokenName,
          'label' => $label,
        ]);
      }
    }
  }

  /**
   * Get metadata about the declared tokens.
   *
   * @return array
   *
   * @throws \CiviCRM_API3_Exception
   */
  public function getTokenMetadata(): array {
    $civiTokens = \Civi::cache()->get('civitoken_enabled_tokens');
    if (!is_array($civiTokens)) {
      $civiTokens = [];
      civitoken_civicrm_tokens_all($civiTokens);
      $setting = civicrm_api3('Setting', 'get', [
        'return' => 'civitoken_enabled_tokens',
        'sequential' => 1,
      ])['values'][0];

      if (empty($setting) || empty($setting['civitoken_enabled_tokens'])) {
        // Treat un-configured as 'all enabled'.
        \Civi::cache()->set('civitoken_enabled_tokens', $civiTokens);
        return $civiTokens;
      }

      foreach ($civiTokens as $category => $tokenSubset) {
        foreach ($tokenSubset as $key => $token) {
          if (!in_array($key, $setting['civitoken_enabled_tokens'], TRUE)) {
            unset($civiTokens[$category][$key]);
          }
        }
        if (empty($civiTokens[$category])) {
          unset($civiTokens[$category]);
        }
      }
      \Civi::cache()->set('civitoken_enabled_tokens', $civiTokens);
    }
    return $civiTokens;
  }

  /**
   * Determine whether this token-handler should be used with
   * the given processor.
   *
   * To short-circuit token-processing in irrelevant contexts,
   * override this.
   *
   * @param \Civi\Token\TokenProcessor $processor
   * @return bool
   */
  public function checkActive(TokenProcessor $processor): bool {
    return in_array($this->getEntityIDField(), $processor->context['schema'], TRUE);
  }

  /**
   * Get the field that would be in the schema context if this class is in use.
   */
  protected function getEntityIDField(): string {
    return 'contactId';
  }

}
