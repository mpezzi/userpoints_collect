<?php

/**
 * @file
 * Provides variable definitions for userpoints_collect module.
 */


/**
 * Implements hook_variable_info().
 */
function userpoints_collect_variable_info($options) {
  $defaults = userpoints_collect_form_defaults();
  $variables['userpoints_collect_submit_label'] = array(
    'type' => 'string',
    'title' => t('Submit Button label', array(), $options),
    'default' => $defaults['submit_label'],
    'description' => t('Label to display on the userpoints_collect submit button', array(), $options),
    'required' => TRUE,
    'group' => 'userpoints_collect',
  );
  $variables['userpoints_collect_return_label'] = array(
    'type' => 'string',
    'title' => t('Return to Site label', array(), $options),
    'default' => $defaults['return_label'],
    'description' => t('Messaging to display on the button prompting user to return to a particular site. This is displayed when no ballots are available.', array(), $options),
    'required' => TRUE,
    'group' => 'userpoints_collect',
  );
  $variables['userpoints_collect_return_url'] = array(
    'type' => 'string',
    'title' => t('Return to Site URL', array(), $options),
    'default' => $defaults['return_url'],
    'description' => t('URL to the site users should be redirected to if no ballots are are available.', array(), $options),
    'required' => TRUE,
    'group' => 'userpoints_collect',
  );
  $variables['userpoints_collect_no_ballots_label'] = array(
    'type' => 'string',
    'title' => t('No ballots remaining label', array(), $options),
    'default' => $defaults['no_ballots_label'],
    'description' => 'Messaging to indicate that there are no more ballots remaining.',
    'required' => TRUE,
    'group' => 'userpoints_collect',
  );
  $variables['userpoints_collect_next_ballot_label'] = array(
    'type' => 'string',
    'title' => t('Next ballot available in... label', array(), $options),
    'description' => 'Messaging for when the next userpoints_collect ballot will be available.',
    'default' => $defaults['next_ballot_label'],
    'required' => TRUE,
    'group' => 'userpoints_collect',
  );
  $variables['userpoints_collect_after_collect_path'] = array(
    'type' => 'string',
    'title' => t('After collect path', array(), $options),
    'description' => 'Redirect to this path after user collects a point',
    'default' => $defaults['after_collect_path'],
    'required' => TRUE,
    'group' => 'userpoints_collect',
  );

  return $variables;
}

/**
 * Implements hook_variable_group_info().
 */
function userpoints_collect_variable_group_info() {
  $groups['userpoints_collect'] = array(
    'title' => t('Userpoints Collect'),
    'description' => t('Userpoints Collect settings.'),
    'access' => 'administer site configuration',
    'path' => array('admin/config/people/userpoints/collect'),
  );
  return $groups;
}
