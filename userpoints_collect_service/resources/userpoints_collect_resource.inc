<?php

/**
 * @file
 * Provides userpoints_collect resource for services module.
 */


/**
 * Determine whether a user is eligible to collect a point.
 *
 * @return
 *   Boolean TRUE if the account is eligible.
 */
function _userpoints_collect_resource_eligibility($tid = NULL) {
  if ( is_null($tid) ) {
    $tid = userpoints_get_default_tid();
  }

  // @TODO: Look into security implications of being able to pass user id as a parameter.
  return userpoints_collect_eligibility($GLOBALS['user'], time(), $tid);
}

/**
 * Add points to a user account.
 */
function _userpoints_collect_resource_add($tid, $operation, $description, $entity_type, $entity_id) {
  global $user;

  if ( is_null($tid) ) {
    $tid = userpoints_get_default_tid('tid');
  }

  $eligibility = userpoints_collect_eligibility($user, time(), $tid);
  if ( $eligibility['status'] == USERPOINTS_COLLECT_IS_ELIGIBLE ) {
    // @TODO: Look into security implications of being able to pass user id or points as a parameter.
    $params = array(
      'uid' => $user->uid,
      'points' => userpoints_collect_config('points'),
      'tid' => $tid,
      'operation' => $operation,
      'description' => $description,
      'entity_type' => $entity_type,
      'entity_id' => $entity_id,
    );

    return userpoints_userpointsapi($params);
  }

  return FALSE;
}

/**
 * Get point total from current user account.
 */
function _userpoints_collect_resource_get($tid = NULL) {
  if ( is_null($tid) ) {
    $tid = 'all';
  }

  return userpoints_get_current_points($GLOBALS['user']->uid, $tid);
}

/**
 * Return user information for the current user.
 */
function _userpoints_collect_resource_account() {
  $account = user_load($GLOBALS['user']->uid);

  services_remove_user_data($account);

  // Load profile fields.
  if ( module_exists('profile2') ) {
    foreach ( profile2_get_types() as $type => $profile_type ) {
      if ( $profile_type->userView && $profile = profile2_load_by_user($account, $type) ) {
        if ( profile2_access('view', $profile) ) {
          $account->profile[$type] = $profile->view('account');
        }
      }
    }
  }

  return $account;
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
  if ( $op == 'eligibility' || $op == 'add' || $op == 'get' || $op == 'account' ) {
    return user_access('use userpoints collect');
  }

  return FALSE;
}
