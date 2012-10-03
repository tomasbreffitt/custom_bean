<?php
/**
 * @file
 * event bean plugin.
 */

class EventBean extends BeanPlugin {
  /**
   * Declares default block settings.
   */
  public function values() {
    return array(
      'settings' => array(
        'event_type' => 1,
      ),
    );
  }

  /**
   * Builds extra settings for the block edit form.
   */
  public function form($bean, $form, &$form_state) {
    $form = array();
    $form['settings'] = array(
      '#type' => 'fieldset',
      '#tree' => 1,
      '#title' => t('Options'),
    );

    $event_types=taxonomy_get_tree(taxonomy_vocabulary_machine_name_load('event_type')->vid);
    foreach($event_types as $event_type){
      $options[$event_type->tid]=$event_type->name;
    }
    $form['settings']['event_type'] = array(
      '#type' => 'select',
      '#title' => t('Event Type'),
      '#description' => t('Select a event type taxonomy term to filter by.'),
      '#options' => $options,
      '#multiple' => TRUE,
      '#default_value' => isset($bean->settings['event_type']) ? $bean->settings['event_type'] : 1,
    );
    $form['settings']['display_count'] = array(
      '#type' => 'select',
      '#title' => t('Number of items to display'),
      '#options' => array(1,2,3,4,5,6,7,8,9,10),
      '#default_value' => isset($bean->settings['display_count']) ? $bean->settings['display_count'] : 9,
    );
    $form['settings']['date2'] = array(
         '#type' => 'date_popup',
         '#title' => t('Start date'),
         '#default_value' => $date_now = date('m/d/Y'),
         '#date_format' => 'm/d/Y',
         '#date_label_position' => 'within',
         '#description' => t('Show events with a start date on or after the above date. If no date is provided, it will display future events'),
      );

    return $form;
  }

  /**
   * Displays the bean.
   */
  public function view($bean, $content, $view_mode = 'default', $langcode = NULL) {

    $view = views_get_view('events_custom');


    $tid['id'] = 'tid';
    $tid['table'] = 'taxonomy_index';
    $tid['field'] = 'tid';
    $tid['relationship'] = 'none';
    $tid['group_type'] = 'group';
    $tid['ui_name'] = '';
    $tid['operator'] = 'or';
    $tid['value'] = $bean->settings['event_type'];
    $tid['exposed'] = FALSE;
    $tid['reduce_duplicates'] = 0;
    $tid['type'] = 'select';
    $tid['limit'] = TRUE;
    $tid['vocabulary'] = 'event_type';
    $tid['hierarchy'] = 0;
    $tid['error_message'] = TRUE;
    $view->display['block_upcoming_custom']->display_options['filters']['tid'] = $tid;

    $date['id'] = 'field_date_range_value';
    $date['table'] = 'field_data_field_date_range';
    $date['field'] = 'field_date_range_value';
    $date['relationship'] = 'none';
    $date['group_type'] = 'group';
    $date['ui_name'] = '';
    $date['operator'] = '>=';
    if($bean->settings['date2'])
      $start = $bean->settings['date2'];
    else
      $start = date('Y-m-d') . ' 00:00';

    $date['value'] = array('min' => NULL, 'max' => NULL, 'value' => $start, 'type' => 'date');
    $date['group'] = 1;
    $date['exposed'] = FALSE;
    $date['granularity'] = 'minute';
    $view->display['block_upcoming_custom']->display_options['filters']['field_date_range_value'] = $date;

    $view->set_items_per_page($bean->settings['display_count'] + 1);

    $view->set_display('block_upcoming_custom');

    $content['event_bean']['#markup'] = $view->preview();

    return $content;
  }
}
