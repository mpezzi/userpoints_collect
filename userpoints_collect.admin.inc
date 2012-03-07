<?php

/**
 * @file
 * Provides administrative settings for userpoints collect module.
 */


/**
 * Administrative Settings Form.
 */
function userpoints_collect_admin_settings($form, &$form_state) {
  $form['userpoints_collect_tid'] = array(
    '#type' => 'select',
    '#title' => t('Category'),
    '#required' => TRUE,
    '#default_value' => userpoints_collect_config('tid'),
    '#options' => userpoints_get_categories(),
  );
  $form['userpoints_collect_points'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of points to collect'),
    '#required' => TRUE,
    '#default_value' => userpoints_collect_config('points'),
  );
  $form['userpoints_collect_is_eligible_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Eligible Button Text'),
    '#required' => TRUE,
    '#default_value' => userpoints_collect_config('is_eligible_text'),
  );
  $form['userpoints_collect_is_not_eligible_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Not Eligible Button Text'),
    '#required' => TRUE,
    '#default_value' => userpoints_collect_config('is_not_eligible_text'),
  );
  $form['userpoints_collect_is_not_eligible_link'] = array(
    '#type' => 'textfield',
    '#title' => t('Not Eligible Button Link'),
    '#required' => TRUE,
    '#default_value' => userpoints_collect_config('is_not_eligible_link'),
  );
  $form['userpoints_collect_note_is_early'] = array(
    '#type' => 'textfield',
    '#title' => t('Note to show when eligibility is early'),
    '#description' => t('Place @start where you would like the start time of the restriction to show.'),
    '#required' => TRUE,
    '#default_value' => userpoints_collect_config('note_is_early'),
  );
  $form['userpoints_collect_note_is_late'] = array(
    '#type' => 'textfield',
    '#title' => t('Note to show when eligibility is late'),
    '#description' => t('Place @end where you would like the end time of the restriction to show.'),
    '#required' => TRUE,
    '#default_value' => userpoints_collect_config('note_is_late'),
  );
  $form['userpoints_collect_note_is_not_eligible'] = array(
    '#type' => 'textfield',
    '#title' => t('Note to show when not eligible'),
    '#description' => t('Place @timeleft where you would like the time left of the restriction to show.'),
    '#required' => TRUE,
    '#default_value' => userpoints_collect_config('note_is_not_eligible'),
  );
  $form['userpoints_collect_redirect'] = array(
    '#type' => 'textfield',
    '#title' => t('Redirect after collecting point'),
    '#description' => 'Redirect to this path after user collects a point.',
    '#required' => TRUE,
    '#default_value' => userpoints_collect_config('redirect'),
  );

  return system_settings_form($form);
}
