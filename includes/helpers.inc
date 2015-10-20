<?php
/**
 * @file
 * Helper functions for commerce_product_sync.
 */

/**
 * Helper: Returns referenceable types.
 *
 * @param array $referenceable_types
 *   Product bundles as keys, enabled as value.
 *
 * @return array
 *   Original array, if all values are 0.
 *   Filtered array, if not all values are 0.
 */
function _commerce_product_sync_filter_referenceable_types($referenceable_types = array()) {
  $return = array();

  // Filter out values that are 0.
  foreach ($referenceable_types as $key => $value) {
    if ($value) {
      $return[$key] = $value;
    }
  }

  // If all values are 0, return original.
  if (empty($return)) {
    $return = $referenceable_types;
  }

  return array_combine(array_keys($return), array_keys($return));
}


/**
 * Returns attribute fields for product bundle.
 */
function _commerce_product_sync_get_attribute_fields($commerce_product_bundle) {
  $attribute_fields = array();

  // Load fields from product bundle and filter attribute-fields.
  $field_info_instances = field_info_instances('commerce_product', $commerce_product_bundle);

  // Add attributes fields to return array.
  foreach ($field_info_instances as $field_machine_name => $field_instance_settings) {
    if (isset($field_instance_settings['commerce_cart_settings']) && $field_instance_settings['commerce_cart_settings']['attribute_field'] == TRUE) {
      $attribute_fields[$field_machine_name] = $field_machine_name;
    }
  }

  return $attribute_fields;
}

/**
 * Helper: Returns a list with possible attributes.
 *
 * Used to calculate cartesian product.
 *
 * @todo: is this working for taxonomy_term references?
 *
 * @param array $attribute_fields
 *    Machine names of fields.
 * @param string $commerce_product_bundle
 *    Commerce product bundle.
 *
 * @return array
 *    Entity-Ids of values that can be referenced. Grouped by attribute_field.
 */
function _commerce_product_sync_get_attribute_fields_possible_values($attribute_fields, $commerce_product_bundle) {
  $return = array();

  foreach ($attribute_fields as $k => &$attribute_field) {
    $field = field_info_field($attribute_field);
    $allowed_values = entityreference_get_selection_handler($field)->getReferencableEntities();

    // Allowed values are grouped by bundle.
    foreach ($allowed_values as $bundle => $entities) {
      foreach ($entities as $id => $label) {
        $return[$attribute_field][$id] = $id;
      }
    }
  }

  return $return;
}

/**
 * Returns all fields, except attributes for product bundle.
 */
function _commerce_product_sync_get_static_fields($commerce_product_bundle) {
  $static_fields = array();

  // Load fields from product bundle and remove filter attribute-fields.
  $field_info_instances = field_info_instances('commerce_product', $commerce_product_bundle);
  $attribute_fields = _commerce_product_sync_get_attribute_fields($commerce_product_bundle);

  // Add static (non-attribute) fields to return array.
  foreach ($field_info_instances as $field_machine_name => $field_instance_settings) {
    if (!in_array($field_machine_name, $attribute_fields)) {
      $static_fields[$field_machine_name] = $field_machine_name;
    }
  }

  return $static_fields;
}

/**
 * Returns array as key_value string.
 */
if (!function_exists('array_to_machine_name')) {

  /**
   * Returns array as key_value string.
   *
   * @param array $array
   *   Like: array(a=>1, b=>2]).
   *
   * @return string
   *   becomes 'a1_a2';
   */
  function array_to_machine_name($array = array()) {
    $string = '';

    $count = 0;
    foreach ($array as $k => $v) {
      if ($count > 0) {
        $string .= "__";
      }
      $count++;

      $string .= (string) $k . (string) $v;
    }

    return (string) $string;
  }
}
