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
 * Debug date format.
 */
define('USERPOINTS_COLLECT_DEBUG_DATE_FORMAT', 'd M Y H:i:s');

/**
 * Load a restriction.
 *
 * @param $rid
 *   A Restriction ID.
 * @return
 *   A Restriction Object or FALSE
 */
function userpoints_collect_restriction_load($rid) {
  $restrictions = &drupal_static(__FUNCTION__, array());

  if ( !isset($restrictions[$rid]) ) {
    $restriction = db_select('userpoints_collect_restrictions', 'r')
      ->fields('r')
      ->condition('r.rid', $rid)
      ->execute()
      ->fetchObject();

    if ( $restriction->type == 'series' ) {
      $restriction->data = db_select('userpoints_collect_restrictions_series', 'rs')
        ->fields('rs')
        ->condition('rs.rid', $rid)
        ->orderBy('timestamp')
        ->execute()
        ->fetchAll();
    }
    elseif ( $restriction->type == 'interval' ) {
      $restriction->data = db_select('userpoints_collect_restrictions_interval', 'ri')
        ->fields('ri')
        ->condition('ri.rid', $rid)
        ->execute()
        ->fetchObject();
    }

    $restrictions[$rid] = $restriction;
  }

  return $restrictions[$rid];
}

/**
 * Load multiple restrictions.
 *
 * @param $param
 *   An array of conditions to match against the database query.
 * @return
 *   An array of restriction objects.
 */
function userpoints_collect_restriction_load_multiple($param = array()) {
  $query = db_select('userpoints_collect_restrictions', 'r')
    ->fields('r', array('rid'));

  foreach ( $param as $field => $value ) {
    $query->condition($field, $value);
  }

  $result = $query->execute();
  $restrictions = array();
  foreach ( $result as $restriction ) {
    $restrictions[] = userpoints_collect_restriction_load($restriction->rid);
  }

  return $restrictions;
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
  db_truncate('userpoints_collect_restrictions_interval')->execute();
  db_truncate('userpoints_collect_restrictions_series')->execute();
  drupal_set_message(t('Cleared all point restrictions'));
}

/**
 * Generate an interval restriction.
 */
function userpoints_collect_restriction_generate_interval($tid = NULL, $start = NULL, $end = NULL, $length = 60) {
  userpoints_collect_restriction_clear_all();

  if ( is_null($tid) ) {
    $tid = userpoints_get_default_tid();
  }

  if ( is_null($start) ) {
    $start = time();
  }

  if ( is_null($end) ) {
    $end = $start + 600;
  }

  $restriction = (object) array(
    'type' => 'interval',
    'tid' => $tid,
    'status' => 1,
  );

  userpoints_collect_restriction_save($restriction);

  db_insert('userpoints_collect_restrictions_interval')
    ->fields(array(
      'rid' => 1,
      'start' => $start,
      'end' => $end,
      'length' => $length,
    ))
    ->execute();

  drupal_set_message(t('Successfully generated 1 interval restriction.'));
}

/**
 * Generate a restriction series.
 *
 * @param $number
 *   The number of series restrictions to generate.
 * @param $length
 *   The duration between restrictions.
 */
function userpoints_collect_restriction_generate_series($tid = NULL, $number = 10, $length = 60) {
  userpoints_collect_restriction_clear_all();

  if ( is_null($tid) ) {
    $tid = userpoints_get_default_tid();
  }

  $restriction = (object) array(
    'type' => 'series',
    'tid' => $tid,
    'status' => 1,
  );

  userpoints_collect_restriction_save($restriction);

  $timestamp = time();
  for ( $i = 1; $i <= $number; $i++ ) {
    $series = (object) array(
      'rid' => $restriction->rid,
      'timestamp' => $timestamp + $i * $length,
      'status' => 1,
    );

    drupal_write_record('userpoints_collect_restrictions_series', $series);
  }

  drupal_set_message(t('Sucessfully generated 1 series restrictions.'));
}

/**
 * Determine whether an account is eligible to collect a point.
 *
 * @param $account
 *   (optional) A user object.
 * @param $timestamp
 *   (optional) A timestamp.
 * @param $tid
 *   (optional) A Category ID.
 * @return
 *   TRUE if the account is eligible to collect a point.
 */
function userpoints_collect_eligibility($account = NULL, $timestamp = NULL, $tid = NULL) {
  $eligible = USERPOINTS_COLLECT_IS_ELIGIBLE;

  if ( is_null($account) ) {
    $account = $GLOBALS['user'];
  }

  if ( is_null($timestamp) ) {
    $timestamp = time();
  }

  if ( is_null($tid) ) {
    $tid = userpoints_get_default_tid();
  }

  $restrictions = userpoints_collect_restriction_load_multiple(array('status' => 1, 'tid' => $tid));

  if ( count($restrictions) > 0 ) {
    $last_txn = userpoints_collect_last_txn($account, $tid);
    foreach ( $restrictions as $restriction ) {
      $function = 'userpoints_collect_eligibility_' . $restriction->type;
      if ( $eligible == USERPOINTS_COLLECT_IS_ELIGIBLE && function_exists($function) ) {
        $eligibility = $function($restriction, $timestamp, $last_txn);
        $eligible = $eligibility['status'];
      }
    }

    // Debug the eligibility object for privileged users.
    userpoints_collect_eligibility_debug($eligibility);

    return $eligibility;
  }

  // Something went wrong.
  return FALSE;
}

/**
 * Returns the time until an account is eligible to collect a point.
 *
 * @param $account
 *   (optional) A user object.
 * @param $timestamp
 *   (optional) A timestamp.
 * @param $tid
 *   (optional) A Category ID.
 * @return
 *   The number of seconds until the account is eligible to collect a point.
 */
function userpoints_collect_eligibility_next($account = NULL, $timestamp = NULL, $tid = NULL) {
  $eligibility = userpoints_collect_eligibility($account, $timestamp, $tid);

  if ( isset($eligibility['max']) ) {
    return $eligibility['max'] - $timestamp;
  }

  return -1;
}

/**
 * Return the amount of time left until an account is eligible to collect a point.
 */
function userpoints_collect_eligibility_next_timeleft($account = NULL, $timestamp = NULL, $tid = NULL) {
  $eligibility = userpoints_collect_eligibility($account, $timestamp, $tid);

  if ( $eligibility['status'] == USERPOINTS_COLLECT_IS_NOT_ELIGIBLE && isset($eligibility['period_end']) ) {
    return format_interval($eligibility['period_end'] - $eligibility['check']);
  }
  elseif ( $eligibility['status'] == USERPOINTS_COLLECT_IS_EARLY ) {
    return format_interval($eligibility['start'] - $eligibility['check']);
  }
  elseif ( $eligibility['status'] == USERPOINTS_COLLECT_IS_LATE ) {
    return t('Too late buddy');
  }

  return '';
}

/**
 * Returns eligibility information for interval based time checking.
 *
 * @param $restriction
 *   A Restriction object.
 * @param $timestamp
 *   A timestamp to check eligibility for.
 * @param $last_txn
 *   The last transaction performed.
 * @return
 *   A constant of USERPOINTS_COLLECT_IS_EARLY, USERPOINTS_COLLECT_IS_LATE, USERPOINTS_COLLECT_IS_ELIGIBLE or USERPOINTS_COLLECT_IS_NOT_ELIGIBLE.
 */
function userpoints_collect_eligibility_interval($restriction, $timestamp, $last_txn) {
  $start = $restriction->data->start;
  $end = $restriction->data->end;
  $length = $restriction->data->length;

  if ( $timestamp < $start ) {
    return array(
      'status' => USERPOINTS_COLLECT_IS_EARLY,
      'check' => $timestamp,
      'start' => $start,
      'end' => $end,
    );
  }
  elseif ( $end != 0 && $timestamp >= $end ) {
    return array(
      'status' => USERPOINTS_COLLECT_IS_LATE,
      'check' => $timestamp,
      'start' => $start,
      'end' => $end,
    );
  }
  else {
    $num_intervals = ( $timestamp - $start ) / $length;

    // Intervals divides into timestamp evenly.
    if ( ( $timestamp - $start ) % $length == 0 ) {
      $min = $start + $num_intervals * $length;
      $max = $start + ( $num_intervals + 1 ) * $length;
    }
    // Intervals isn't a whole number calculate max and min values.
    else {
      $min_num_intervals = floor($num_intervals);
      $max_num_intervals = ceil($num_intervals);
      $min = $start + $min_num_intervals * $length;
      $max = $start + $max_num_intervals * $length;
    }

    if ( $timestamp >= $min && $timestamp < $max ) {
      if ( !isset($last_txn->time_stamp) || ( isset($last_txn->time_stamp) && $last_txn->time_stamp < $min ) ) {
        return array(
          'status' => USERPOINTS_COLLECT_IS_ELIGIBLE,
          'check' => $timestamp,
          'last' => isset($last_txn->time_stamp) ? $last_txn->time_stamp : 0,
          'start' => $start,
          'end' => $end,
          'period_start' => $min,
          'period_end' => $max,
        );
      }
    }
  }

  return array(
    'status' => USERPOINTS_COLLECT_IS_NOT_ELIGIBLE,
    'check' => $timestamp,
    'last' => isset($last_txn->time_stamp) ? $last_txn->time_stamp : 0,
    'start' => $start,
    'end' => $end,
    'period_start' => $min,
    'period_end' => $max,
  );
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
 *   A constant of USERPOINTS_COLLECT_IS_EARLY, USERPOINTS_COLLECT_IS_LATE, USERPOINTS_COLLECT_IS_ELIGIBLE or USERPOINTS_COLLECT_IS_NOT_ELIGIBLE.
 */
function userpoints_collect_eligibility_series($restriction, $timestamp, $last_txn) {
  $s = count($restriction->data);
  $series = $restriction->data;
  $start = $series[0]->timestamp;
  $end = $series[$s - 1]->timestamp;

  // Too early, not available.
  if ( $timestamp < $start ) {
    return array(
      'status' => USERPOINTS_COLLECT_IS_EARLY,
      'check' => $timestamp,
      'start' => $start,
      'end' => $end,
    );
  }
  // Too late, not available.
  elseif ( $timestamp >= $end ) {
    return array(
      'status' => USERPOINTS_COLLECT_IS_LATE,
      'check' => $timestamp,
      'start' => $start,
      'end' => $end,
    );
  }
  // In between ...
  else {
    for ( $i = 0; $i < $s - 1; $i++ ) {
      if ( $timestamp >= $series[$i]->timestamp && $timestamp < $series[$i + 1]->timestamp ) {
        $min = $series[$i]->timestamp;
        $max = $series[$i + 1]->timestamp;

        if ( !isset($last_txn->time_stamp) || ( isset($last_txn->time_stamp) && $last_txn->time_stamp < $series[$i]->timestamp ) ) {
          return array(
            'status' => USERPOINTS_COLLECT_IS_ELIGIBLE,
            'check' => $timestamp,
            'last' => isset($last_txn->time_stamp) ? $last_txn->time_stamp : 0,
            'start' => $start,
            'end' => $end,
            'period_start' => $min,
            'period_end' => $max,
          );
        }
      }
    }
  }

  return array(
    'status' => USERPOINTS_COLLECT_IS_NOT_ELIGIBLE,
    'check' => $timestamp,
    'last' => isset($last_txn->time_stamp) ? $last_txn->time_stamp : 0,
    'start' => $start,
    'end' => $end,
    'period_start' => $min,
    'period_end' => $max,
  );
}

/**
 * Returns the last userpoints transaction from an account.
 *
 * @param $account
 *   A user object.
 * @param $tid
 *   A Category ID.
 * @return
 *   A userpoints transaction.
 */
function userpoints_collect_last_txn($account, $tid = NULL) {
  $txns = drupal_static(__FUNCTION__, array());

  if ( is_null($tid) ) {
    $tid = userpoints_get_default_tid();
  }

  if ( !isset($txns[$account->uid][$tid]) ) {
      $query = db_select('userpoints_txn', 'u')
      ->fields('u')
      ->condition('uid', $account->uid)
      ->orderBy('time_stamp', 'DESC')
      ->range(0, 1);

    if ( $tid !== 0 ) {
      $query->condition('tid', $tid);
    }

    $txns[$account->uid][$tid] = $query->execute()->fetchObject();
  }

  return $txns[$account->uid][$tid];
}

/**
 * Debug eligibility.
 */
function userpoints_collect_eligibility_debug($eligibility) {
  if ( user_access('debug userpoints collect') ) {
    if ( isset($eligibility['status']) ) {
      drupal_set_message(t('Eligibility: @status', array('@status' => $eligibility['status'], 'custom', USERPOINTS_COLLECT_DEBUG_DATE_FORMAT)));
    }
    if ( isset($eligibility['check']) ) {
      drupal_set_message(t('Check: @time', array('@time' => format_date($eligibility['check'], 'custom', USERPOINTS_COLLECT_DEBUG_DATE_FORMAT))));
    }
    if ( isset($last_txn->time_stamp) ) {
      drupal_set_message(t('Last transaction: @time', array('@time' => format_date($last_txn->time_stamp, 'custom', USERPOINTS_COLLECT_DEBUG_DATE_FORMAT))));
    }
    if ( isset($eligibility['start']) ) {
      drupal_set_message(t('Restriction Start Time: @time', array('@time' => format_date($eligibility['start'], 'custom', USERPOINTS_COLLECT_DEBUG_DATE_FORMAT))));
    }
    if ( isset($eligibility['end']) ) {
      drupal_set_message(t('Restriction End Time: @time', array('@time' => format_date($eligibility['end'], 'custom', USERPOINTS_COLLECT_DEBUG_DATE_FORMAT))));
    }
    if ( isset($eligibility['period_start']) ) {
      drupal_set_message(t('Period Start: @time', array('@time' => format_date($eligibility['period_start'], 'custom', USERPOINTS_COLLECT_DEBUG_DATE_FORMAT))));
    }
    if ( isset($eligibility['period_end']) ) {
      drupal_set_message(t('Period End: @time', array('@time' => format_date($eligibility['period_end'], 'custom', USERPOINTS_COLLECT_DEBUG_DATE_FORMAT))));
    }
  }
}
