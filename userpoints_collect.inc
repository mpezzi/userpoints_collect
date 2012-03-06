<?php

/**
 * @file
 * Provides internal functionality for userpoints collect.
 */


/**
 * Eligibility is early.
 */
define('USERPOINTS_COLLECT_IS_EARLY', 1);

/**
 * Eligibility is late.
 */
define('USERPOINTS_COLLECT_IS_LATE', 2);

/**
 * Eligibility in range.
 */
define('USERPOINTS_COLLECT_IS_ELIGIBLE', 3);

/**
 * Eligibility not in range.
 */
define('USERPOINTS_COLLECT_IS_NOT_ELIGIBLE', 4);

/**
 * Load a restriction.
 *
 * @param $rid
 *   A Restriction ID.
 * @return
 *   A Restriction Object or FALSE
 */
function userpoints_collect_restriction_load($rid) {
  $restrictions = drupal_static(__FUNCTION__, array());

  if ( !isset($restrictions[$rid]) ) {
    $restriction = db_select('userpoints_collect_restrictions', 'r')
      ->fields('r')
      ->condition('r.rid', $rid)
      ->execute()
      ->fetchObject();

    if ( $restriction->type == 'series' ) {
      $restriction->series = db_select('userpoints_collect_restrictions_series', 'rs')
        ->fields('rs')
        ->condition('rs.rid', $rid)
        ->orderBy('timestamp')
        ->execute()
        ->fetchAll();
    }

    $restrictions[$rid] = $restriction;
  }

  return $restrictions[$rid];
}

/**
 * Save a restriction.
 *
 * @param $restriction
 *   A Restriction Object.
 */
function userpoints_collect_restriction_save(&$restriction) {
  $restriction->is_new = empty($restriction->rid);

  if ( $restriction->is_new ) {
    $status = drupal_write_record('userpoints_collect_restrictions', $restriction);
  }
  else {
    $status = drupal_write_record('userpoints_collect_restrictions', $restriction, 'rid');
  }
}

/**
 * Clear all restrictions.
 */
function userpoints_collect_restriction_clear_all() {
  db_truncate('userpoints_collect_restrictions')->execute();
  db_truncate('userpoints_collect_restrictions_series')->execute();
  drupal_set_message(t('Cleared all point restrictions'));
}

/**
 * Generate a restriction series.
 *
 * @param $number
 *   The number of series restrictions to generate.
 * @param $duration
 *   The duration between restrictions.
 */
function userpoints_collect_restriction_generate_series($number = 10, $duration = 60) {
  userpoints_collect_restriction_clear_all();

  $restriction = (object) array(
    'type' => 'series',
    'tid' => 0,
    'status' => 1,
  );

  userpoints_collect_restriction_save($restriction);

  $timestamp = time();
  for ( $i = 1; $i <= $number; $i++ ) {
    $series = (object) array(
      'rid' => $restriction->rid,
      'timestamp' => $timestamp + $i * 60,
      'status' => 1,
    );

    drupal_write_record('userpoints_collect_restrictions_series', $series);
  }
}

/**
 * Returns eligibility information for series based time checking.
 *
 * @param $restriction
 *   A Restriction object.
 * @param $timestamp
 *   A timestamp to check eligibility for.
 * @param $last_txn
 *   The last transaction performed.
 * @return
 *   An array containing information about the
 */
function userpoints_collect_eligibility_series($restriction, $timestamp, $last_txn) {
  $s = count($restriction->series);
  $series = $restriction->series;

  // Too early, not available.
  if ( $timestamp < $series[0]->timestamp ) {
    return USERPOINTS_COLLECT_IS_EARLY;
  }
  // Too late, not available.
  elseif ( $timestamp >= $series[$s - 1]->timestamp ) {
    return USERPOINTS_COLLECT_IS_LATE;
  }
  // In between ...
  else {
    for ( $i = 0; $i < $s - 1; $i++ ) {
      if ( $timestamp >= $series[$i]->timestamp && $timestamp < $series[$i + 1]->timestamp ) {
        if ( !isset($last_txn->time_stamp) || ( isset($last_txn->time_stamp) && $last_txn->time_stamp < $series[$i] && $last_txn->time_stamp > $series[$i + 1] ) ) {
          return USERPOINTS_COLLECT_IS_ELIGIBLE;
        }
      }
    }
  }

  return USERPOINTS_COLLECT_IS_NOT_ELIGIBLE;
}
