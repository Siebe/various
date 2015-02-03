<?php


class OP {
    
    static private $reserved = array(
        'werk', 'nieuws', 'contact', 'over', 'details', 'afbeelding', 'backend'
    );
    
    static public function getObjectByIdOrTitle($model, $param, $first_default = true) {
        $object = null;
        if (isset($param['reference'])) {
            $ref = $param['reference'];

            if (is_numeric($ref)) {
                $object = R::load($model, $ref);

            } else {
                $object = R::findOne($model, "url_title LIKE ?", array($ref));
            }
        }

        if ($first_default &&
            (!$object ||
            $object->id < 1)
        ) {
            $object = R::load($model, 1);
            if ($object->id < 1) {
                return false;
            }
        }
        return $object;

    }
    
    public static function getUrlTitle($object) {
        if (!isset($object->title) || $object->title == '' || 
            (isset($object->url_title) && $object->url_title != '')
        ) {
            return;
        }
        $url_title = self::slugify($object->title);
        $counter = 0;
        while($counter++ <= 1000) {
            
            $results = R::getAll("
(SELECT work.url_title 
FROM work WHERE work.url_title = '".$url_title."')
UNION
(SELECT post.url_title
FROM post WHERE post.url_title = '".$url_title."')"
            );
            if (sizeof($results) < 1 && !array_search($url_title, self::$reserved)) {
                break;
            }
            $url_title = ($counter == 1000) ? $url_title : $url_title.$counter;
        }
        return $url_title;
    }
    
    
    static public function slugify($text)
    { 
      // replace non letter or digits by -
      $text = preg_replace('~[^\\pL\d]+~u', '-', $text);
      $text = trim($text, '-');
      // transliterate
      $text = iconv('utf-8', 'us-ascii//TRANSLIT', $text);
      $text = strtolower($text);
      // remove unwanted characters
      $text = preg_replace('~[^-\w]+~', '', $text);
      if (empty($text))
      {
        return 'n-a';
      }
      return $text;
    }

    
}

?>
