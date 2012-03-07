<?php

/**
 * @file
 * Provides variable definitions for userpoints_collect module.
 */


/**
 * Implements hook_variable_info().
 */
function userpoints_collect_variable_info($options) {
  $variables['userpoints_collect_submit_label'] = array(
    'type' => 'string',
    'title' => t('Submit Button label', array(), $options),
    'description' => t('Messaging to display on the userpoints_collect submit button', array(), $options),
    'default' => 'Collect now!',
    'required' => TRUE,
    'group' => 'userpoints_collect',
  );
  $variables['userpoints_collect_return_label'] = array(
    'type' => 'string',
    'title' => t('Return to Site label', array(), $options),
    'description' => t('Messaging to display on the button prompting user to return to a particular site. This is displayed when no ballots are available.', array(), $options),
    'default' => 'Back to Facebook',
    'required' => TRUE,
    'group' => 'userpoints_collect',
  );
  $variables['userpoints_collect_return_url'] = array(
    'type' => 'string',
    'title' => t('Return to Site URL', array(), $options),
    'description' => t('URL to the site users should be redirected to if no ballots are available.', array(), $options),
    'default' => 'http://facebook.com',
    'required' => TRUE,
    'group' => 'userpoints_collect',
  );
  $variables['userpoints_collect_no_ballots_label'] = array(
    'type' => 'string',
    'title' => t('No ballots remaining label', array(), $options),
    'description' => 'Messaging to indicate that there are no more ballots remaining.',
    'default' => 'No more ballots remaining',
    'required' => TRUE,
    'group' => 'userpoints_collect',
  );
  $variables['userpoints_collect_next_ballot_label'] = array(
    'type' => 'string',
    'title' => t('Next ballot available in... label', array(), $options),
    'description' => 'Messaging for when the next userpoints_collect ballot will be available.',
    'default' => 'Next ballot available in @timeleft',
    'required' => TRUE,
    'group' => 'userpoints_collect',
  );
  $variables['userpoints_collect_redirect'] = array(
    'type' => 'string',
    'title' => t('After collect path', array(), $options),
    'description' => 'Path to redirect user to after collecting a point',
    'default' => '<front>',
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
