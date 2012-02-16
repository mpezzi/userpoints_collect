<?php

/**
 * @file
 * Callbacks and access callbacks for userpoints service integration.
 */


/**
 * Determine access for userpoints collect service.
 */
function userpoints_collect_service_access($op, $uid = NULL) {
  global $user;

  // If uid is not supplied use the current logged in user.
  if ( !isset($uid) ) {
    $account = $user;
  }
  else {
    $account = user_load($uid);
  }

  // To check eligiblility, all we need to check is that the user has access to collect points.
  if ( $op == 'eligible' ) {
    return user_access('use userpoints collect', $account);
  }

  // To collet a point, check that the user has access to collect.
  if ( $op == 'add' ) {
    return userpoints_collect_access($account);
  }
}

/**
 * Return if an account is eligible to collect userpoints.
 */
function userpoints_collect_service_eligible($uid) {
  return userpoints_collect_access(user_load($uid));
}

/**
 * Add points to a user if eligible.
 */
function userpoints_collect_service_add($uid, $points, $tid, $operation, $description, $entity_type, $entity_id) {
  if ( !$uid ) {
    return services_error(t('User ID parameter is required.'));
  }

  if ( !$points ) {
    return services_error(t('Points parameter must be a negative or positive number.'))
  }

  $params = array(
    'uid' => $uid,
    'points' => $points,
    'tid' => $tid,
    'operation' => $operation,
    'description' => $description,
    'entity_type' => $entity_type,
    'entity_id' => $entity_id,
  );

  $result = userpoints_collect_add($params);

  if ( !$result['status'] ) {
    return services_error(t('Adding points failed: @reason', array('@reason' => $result['reason'])));
  }

  return (object) array(
    'id' => $result['transaction']['txn_id'],
    'uri' => services_resource_uri(array('userpoints_transaction', $result['transaction']['txn_id'])),
  );
}
