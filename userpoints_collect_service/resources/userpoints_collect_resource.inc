<?php

/**
 * @file
 * Provides userpoints_collect resource for services module.
 */

/**
 * Returns the results of seed_load() for the specified seed.
 *
 * @param $sid
 *   A Seed ID.
 * @return
 *   Seed object or FALSE if not found.
 */
function _userpoints_collect_resource_retrieve($sid) {
  return array('points' => 1);
}

/**
 * Determine whether a user is eligible to collect a point.
 *
 * @return
 *   Boolean TRUE if the account is eligible.
 */
function _userpoints_collect_resource_eligibility() {
  // @TODO: Look into security implications of being able to pass user id as a parameter.
  return userpoints_collect_eligibility();
}

/**
 * Add points to a user account.
 */
function _userpoints_collect_resource_add($userpoints = array()) {
  $userpoints['uid'] = $GLOBALS['user']->uid;
  $userpoints['points'] = userpoints_collect_config('points');

  // @TODO: Look into security implications of being able to pass user id or points as a parameter.
  return userpoints_collect_add($userpoints);
}

/**
 * Get point total from current user account.
 */
function _userpoints_collect_resource_get($tid = NULL) {
  global $user;

  if ( is_null($tid) ) {
    $tid = userpoints_get_default_tid();
  }

  return userpoints_get_current_points($user->uid, $tid);
}

/**
 * Determine whether the current user can access a userpoints_collect resource.
 *
 * @param $op
 *   An operation type of eligibility or add.
 * @param $args
 *   Resource arguments passed through from the original request.
 * @return
 *   Boolean TRUE if the current user has the requested permissions.
 */
function _userpoints_collect_resource_access($op = 'eligibility', $args = array()) {
  if ( $op == 'eligibility' || $op == 'add' || $op == 'get' ) {
    return user_access('use userpoints collect');
  }

  return FALSE;
}
