<?php
// $Id: yboss.search.inc,v 1.1.2.1 2008/07/14 06:59:28 mfb Exp $

/**
 * Core search module integration.
 */
function yboss_search_process($keys) {
  global $pager_page_array, $pager_total, $pager_total_items, $language;
  $results = array();
  $limit = max(1, intval(variable_get('yboss_limit', 15)));
  $page = isset($_GET['page']) ? intval($_GET['page']) : 0;
  $extra_keys = variable_get('yboss_query', '');
  $keys .= $extra_keys ? ' '. $extra_keys : '';
  $query = array(
    'count' => $limit,
    'start' => $page * $limit,
  );
  $resultset = yboss_search_query($keys, $query);
  foreach ($resultset['items'] as $item) {
    $results[] = array(
      'link' => $item->clickurl,
      'snippet' => filter_xss(str_replace(array('<b>', '</b>'), array('<strong>', '</strong>'), $item->abstract)),
      'title' => html_entity_decode(strip_tags($item->title), ENT_QUOTES, 'UTF-8'),
      'date' => strtotime($item->date . ' UTC'),
      'extra' => array(l(filter_xss($item->dispurl), $item->clickurl, array('html' => TRUE)), format_size(intval($item->size))),
    );
  }
  $element = 0;
  $pager_page_array = explode(',', $page);
  $pager_total_items[$element] = $resultset['total'];
  $pager_total[$element] = ceil($pager_total_items[$element] / $limit);
  $pager_page_array[$element] = max(0, min(intval($pager_page_array[$element]), intval($pager_total[$element]) - 1));
  return $results;
}
