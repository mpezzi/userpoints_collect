<?php

/**
 * @file
 * Provides administrative settings for userpoints collect module.
 */


/**
 * Administrative Settings Form.
 */
function userpoints_collect_admin_settings($form, &$form_state) {
  $defaults = userpoints_collect_form_defaults();

  $form['userpoints_collect_submit_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Submit Button label'),
    '#required' => TRUE,
    '#default_value' => variable_get('userpoints_collect_submit_label', $defaults['submit_label']),
  );
  $form['userpoints_collect_return_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Return to Site label'),
    '#required' => TRUE,
    '#default_value' => variable_get('userpoints_collect_return_label', $defaults['return_label']),
  );
  $form['userpoints_collect_return_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Return to Site URL'),
    '#required' => TRUE,
    '#default_value' => variable_get('userpoints_collect_return_url', $defaults['return_url']),
  );
  $form['userpoints_collect_no_ballots_label'] = array(
    '#type' => 'textfield',
    '#title' => t('No ballots remaining label'),
    '#required' => TRUE,
    '#default_value' => variable_get('userpoints_collect_no_ballots_label', $defaults['no_ballots_label']),
  );
  $form['userpoints_collect_next_ballot_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Next ballot available in... label'),
    '#description' => 'Label must contain @timeleft to indicate where time left until next ballot should be displayed.',
    '#required' => TRUE,
    '#default_value' => variable_get('userpoints_collect_next_ballot_label', $defaults['next_ballot_label']),
  );
  $form['userpoints_collect_after_collect_path'] = array(
    '#type' => 'textfield',
    '#title' => t('After collect path'),
    '#description' => 'Redirect to this path after user collects a point.',
    '#required' => TRUE,
    '#default_value' => variable_get('userpoints_collect_after_collect_path', $defaults['after_collect_path']),
  );

  return system_settings_form($form);
}
