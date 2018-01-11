(function(angular, $, _) {

  angular.module('civitoken').config(function($routeProvider) {
      $routeProvider.when('/civitoken/settings', {
        controller: 'CivitokenSetting',
        templateUrl: '~/civitoken/Setting.html',

        // If you need to look up data when opening the page, list it out
        // under "resolve".
        resolve: {
          existingSettings: function(crmApi) {
            return crmApi('Setting', 'get', {
              return: ['civitoken_enabled_tokens'],
              sequential: true
            });
          },

          settingMetaData: function(crmApi) {
            return crmApi('Setting', 'getfields', {
            });
          },

          settingOptions: function(crmApi) {
            return crmApi('Setting', 'getoptions', {
              'field' : 'civitoken_enabled_tokens',
            });
          }

        }
      });
    }
  );

  // The controller uses *injection*. This default injects a few things:
  //   $scope -- This is the set of variables shared between JS and HTML.
  //   crmApi, crmStatus, crmUiHelp -- These are services provided by civicrm-core.
  //   existingSetting -- The current contact, defined above in config().
  angular.module('civitoken').controller('CivitokenSetting', function($scope, crmApi, crmStatus, crmUiHelp, existingSettings, settingMetaData, settingOptions) {
    // The ts() and hs() functions help load strings for this module.
    var ts = $scope.ts = CRM.ts('civitoken');
    var hs = $scope.hs = crmUiHelp({file: 'CRM/civitoken/Setting'}); // See: templates/CRM/civitoken/Setting.hlp

    // We don't really use the metadata array very fully yet but my intent
    // was to make this form fully metadata driven & hence I have not removed the
    // bits that would help move that way.
    settingMetaData = settingMetaData['values']['civitoken_enabled_tokens'];
    settingOptions = settingOptions['values'];
    existingSettings = existingSettings['values'][0]['civitoken_enabled_tokens'];
    settingMetaData['options'] = {};

    var optionValues = Object.keys(settingOptions);
    optionValues.forEach(function(keyName) {
      settingMetaData['options'][keyName] = {};
      settingMetaData['options'][keyName]['name'] = keyName;
      settingMetaData['options'][keyName]['title'] = settingOptions[keyName];
      settingMetaData['options'][keyName]['selected'] = ($.inArray(keyName, existingSettings) >= 0);
    });
    settingOptions = settingMetaData.options;

    // We have existingSetting available in JS. We also want to reference it in HTML.
    $scope.settingOptions = settingOptions;

    $scope.save = function save() {
      var selected = {'civitoken_enabled_tokens' : []};
      optionValues.forEach(function(keyName) {
        console.log(keyName + settingOptions[keyName]['selected']);
        if (settingOptions[keyName]['selected']) {
          selected['civitoken_enabled_tokens'].push(keyName);
        }
      });
      console.log(selected);
      return crmStatus(
        // Status messages. For defaults, just use "{}"
        {start: ts('Saving...'), success: ts('Saved')},
        // The save action. Note that crmApi() returns a promise.
        crmApi('Setting', 'create', selected)
      );
    };
  });

})(angular, CRM.$, CRM._);
