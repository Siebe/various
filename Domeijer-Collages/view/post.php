<?php include 'header.php';?>
<h3>Nieuws</h3>
<?php
if (!isset($all_post) || sizeof($all_post) < 1) {
    echo '<p>Geen nieuws gevonden</p>';
} else {
    foreach ($all_post as $post) {
        echo '<div class="post-text">';
        echo '<p class="post-title"><b>'.$post->title.'</b></p>';
        echo '<p class="post-created"><b>Geplaatst op:</b>   '.strftime('%#d %b %Y', strtotime($post->created_at)).'</p>';
        echo '<hr>';
        echo '<div class="post-body">'.$post->content.'</div>';
        echo '</div>';
    }
}
?>

<?php include 'footer.php';?>