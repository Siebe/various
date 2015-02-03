<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Description of ImageHelper
 *
 * @author User
 */
class ImageHelper {
    
        private static $last_img_type;
    
        public static function getAndSave($name_prefix = null, $controller = null) {
        try {
            //http://php.net/manual/en/features.file-upload.php

            //var_dump($_FILES);
            
            if (!isset($_FILES['image']['error'])) {
                return null;
            }
            
            //multiple files?
            if (is_array($_FILES['image']['error'])) {
                throw new RuntimeException('Meerdere fouten bij upload, 1 afbeelding per keer.');
            }

            switch ($_FILES['image']['error']) {
                case UPLOAD_ERR_OK:
                    break;
                case UPLOAD_ERR_NO_FILE:
                    return null;
                case UPLOAD_ERR_INI_SIZE:
                case UPLOAD_ERR_FORM_SIZE:
                    throw new RuntimeException('Bestand te groot.');
                default:
                    throw new RuntimeException('Onbekende fout bij uploaden');
            }
            
            $array = explode('.',$_FILES['image']['name']);
            $ext = strtolower(array_pop($array));
            $img_type = exif_imagetype($_FILES['image']['tmp_name']);
          
            if (!$img_type || $img_type > 4) {
                throw new RuntimeException('Bestand is geen geaccepteerde afbeelding (jpg/png/gif).');
            }
            
            self::$last_img_type = $img_type;
            $exif = exif_read_data($_FILES['image']['tmp_name']);
            $orientation = isset($exif['Orientation']) ? $exif['Orientation'] : $exif['IFD0']['Orientation'];
               
            switch($img_type) {
                case "1" : if ($ext == "gif") break;
                case "2" : if ($ext == "jpg" || $ext == "jpeg") break;
                case "3" : if ($ext == "png") break;
                default: throw new RuntimeException('Bestandsextensie klopt niet met formaat (jpg/png/gif).');             
            }
            
            $new_name = (isset($name_prefix) ? $name_prefix."_" : "unkown_").substr(md5(rand(0, 9999999999)), 0, 8);
            $new_file = sprintf(DIRECTORY_SEPARATOR.'content'.DIRECTORY_SEPARATOR.'images'.DIRECTORY_SEPARATOR.'%s.%s', 
                $new_name,
                $ext
            );
            
            $filename = __DIR__.DIRECTORY_SEPARATOR.'..'.$new_file;

            if (!move_uploaded_file($_FILES['image']['tmp_name'],$filename)) {
                $new_file = false;
                throw new RuntimeException('Afbeelding niet kunnen opslaan.');
            }
            
            $image = self::fileToImg($filename);
            
            //fix orientation if needed, for some camera's like my dad's
            if ($orientation > 1) {
                try {
                    switch($orientation) {
                        case 3:
                            $image = imagerotate($image, 180, 0);
                            break;
                        case 6:
                            $image = imagerotate($image, -90, 0);
                            break;
                        case 8:
                            $image = imagerotate($image, 90, 0);
                    }
                    
                } catch (Exception $e) {
                   throw new RuntimeException('Afbeelding niet kunnen draaien.<br>'.$e->getMessage());
                }
            }
            //always 're-save' image to get a quality of 90%
            //TODO preserve metadata (iptc)
            self::imgToFile($image, $filename);
            //get height and width for preview and icon
            $image_size = getimagesize($filename);
            //find out what size index is smaller (0=width)
            $size_index = ($image_size[0] > $image_size[1]) ? 0 : 1;
            
            $icon_scaled = self::scale($image, $size_index ? 100 : 0, $size_index ? 0 : 100);
            $icon_image = self::cropCenter($icon_scaled, 100, 100);
            $icon_file = sprintf(DIRECTORY_SEPARATOR.'content'.DIRECTORY_SEPARATOR.'images'.DIRECTORY_SEPARATOR.'icons'.DIRECTORY_SEPARATOR.'%s.%s', 
                $new_name,
                $ext
            );
            self::imgToFile($icon_image, __DIR__.DIRECTORY_SEPARATOR.'..'.$icon_file, 50);
            
            //get the preview dimensions 
            $preview_size = array(Config::$slider_width, Config::$slider_height);
            //only create preview if both widht and height are more than preview size
            if ($image_size[0] > $preview_size[0] && $image_size[1] > $preview_size[1]) {
                //recalculate what index to use, depending on preview ratio
                $preview_ratio = $preview_size[0] / $preview_size[1];
                $size_index = ($image_size[0] / $preview_ratio > $image_size[0] * $preview_ratio) ? 0 : 1;
                
                $preview_scaled = self::scale($image, $size_index ? $preview_size[0] : 0, $size_index ? 0 : $preview_size[1]);
                if (Config::$crop_preview) {
                    $preview_image = self::cropCenter($preview_scaled, $preview_size[0], $preview_size[1]);
                } else {
                    $preview_image = $preview_scaled;
                }
            } else {
                $preview_image = $image;
            }
            
            $preview_file = sprintf(DIRECTORY_SEPARATOR.'content'.DIRECTORY_SEPARATOR.'images'.DIRECTORY_SEPARATOR.'previews'.DIRECTORY_SEPARATOR.'%s.%s', 
                $new_name,
                $ext
            );
            self::imgToFile($preview_image, __DIR__.DIRECTORY_SEPARATOR.'..'.$preview_file, 50);            
            
            return $new_file;
            
        } catch (RuntimeException $e) {
            //leave an image error at the controller
            if (isset($controller)) {
                $controller->image_error = $e->getMessage();
                return isset($new_file) ? $new_file : false;
            }
            throw $e;
        }
    }
    
    
    private static function fileToImg($filename) {
        $img_type = self::$last_img_type;
        try {
            switch($img_type) {
                case "1" : 
                    $image = imagecreatefromgif($filename);
                    break;
                case "3" : 
                    $image = imagecreatefrompng($filename);
                    break;
                default: 
                    $image = imagecreatefromjpeg($filename);
            }
        } catch (Exception $e) {
            throw new RuntimeException("Afbeelding niet kunnen converteren.<br>".$e->message());
        }
        
        return $image;
    }
    
    private static function imgToFile($image, $filename, $quality = 90) {
        $img_type = self::$last_img_type;
        try {
            switch($img_type) {
               case "1" : 
                   imagegif($image, $filename, $quality);
                   break;
               case "3" : 
                   imagepng($image, $filename, $quality);
                   break;
               default: 
                   imagejpeg($image, $filename, $quality);
           }
        } catch (Exception $e) {
            throw new RuntimeException("Geconverteerde afbeelding niet kunnen opslaan.<br>".$e->message());
        }
    }
    
    private static function scale($image, $width = false, $height = false) {
        if (!$width && !$height) {
            throw new Exception("Fout in scale!");
        }
        
        $src_x = imagesx($image);
        $src_y = imagesy($image);
        
        if (!$width) {
            $width = round($src_x * ($height/$src_y));
        }
        if (!$height) {
            $height = round($src_y * ($width/$src_x));
        }
        
        $dest_image = imagecreatetruecolor($width, $height);

	imagecopyresampled($dest_image, $image, 0, 0, 0, 0, $width, $height, $src_x, $src_y);
        
        return $dest_image;
        
    }
    
    private static function cropCenter($image, $width, $height) {
        
        $src_x = imagesx($image);
        $src_y = imagesy($image);
        
        //find the offset by getting difference between ogiginal and new size,
        //bitshift instead of rounded divide by 2 (geekout!), minimum 0
        $x = max(($src_x - $width) >> 1, 0);
        $y = max(($src_y - $height) >> 1, 0);
        
        /* Doesn't work on php 5.4
        return imagecrop($image, array(
            'x' => $x, 
            'y' => $y, 
            'width' => $width, 
            'height' =>$height
        ));
         */
        $dest_image = imagecreatetruecolor($width, $height);
        imagecopyresized($dest_image, $image, 
                0, 0, $x, $y, 
                $width, $height, $width, $height
        );
        
        return $dest_image;
        
    }
    
    public static function delete($filename, $controller = null) {
        $dir = __DIR__.DIRECTORY_SEPARATOR.'..'.DIRECTORY_SEPARATOR;
        
        if (!file_exists($dir.$filename)) {
            if (isset($controller)) {
                $controller->image_error = "Bestand om te verwijderen niet gevonden: ".$filename;
            }
            return true;
        }
        if (!getimagesize($dir.$filename)) {
            if (isset($controller)) {
                $controller->image_error = "Bestand om te verwijderen is geen afbeelding: ".$filename;
            }
            return false;
        }
        
        $icon = self::getIconFile($filename);
        $preview = self::getPreviewFile($filename);

        
        try {
            unlink($dir.$filename);
        } catch (Exception $e) {
            if (isset($controller)) {
                $controller->image_error = "Afbeelding kon niet verwijderd worden.<br>".$e->getMessage();
            }
            return false;
        }
        
        try {
            if (is_file($dir.$icon)) {
                unlink($dir.$icon);
            }
            if (is_file($dir.$preview)) {
                unlink($dir.$preview);
            }
        }   catch (Exception $e) {
            if (isset($controller)) {
                $controller->image_error = "Kon icoon of preview niet verwijderen.<br>".$e->getMessage();
            }
        }
        return true;
    }
    
    private static function getImageInSubFolder($filename, $subfolder) { 
        $array = explode(DIRECTORY_SEPARATOR, $filename);
        $file = end($array);
        return 'content'.DIRECTORY_SEPARATOR.'images'.DIRECTORY_SEPARATOR.$subfolder.DIRECTORY_SEPARATOR.$file;
    }
    
    public static function getIconFile($filename) {
        return self::getImageInSubFolder($filename, 'icons');
    }
    
    public static function getPreviewFile($filename) {
        return self::getImageInSubFolder($filename, 'previews');
    }
    
    private static function url($filename) {
        return preg_replace("/\\\/", "/", $filename);   
    }
    
    public static function icon($image) {
        return "/".self::url(self::getIconFile($image));
    }
    
    public static function preview($image) {
        return "/".self::url(self::getPreviewFile($image));
    }
    
}

?>
