<?php include('header.php') ?>

<h3 id="work-title"><?php echo $work->title ?></h3>

<div class="details-menu">
    <a href="<?php echo $previous_url ?>">
        <div class="button details-button">
            Vorige
        </div>
    </a>
    <a href="/">
        <div class="button details-button">
            Terug
        </div>
    </a>
    <a href="<?php echo $next_url ?>">
        <div class="button details-button">
            Volgende
        </div>
    </a>
</div>
<div class="menu-spacer"></div>

<div class="details-text">
    <p  class="work-created"><b>Toegevoegd op: </b><?php echo strftime('%#d %b %Y', strtotime($work->created_at)) ?></p>
    <p  class="work-description"><b>Beschrijving:</b><br>
    <?php echo nl2br($work->description)?><p>
</div>

<div class="details-small-text"><p>
        Klik op een afbeelding voor volledig formaat<br>
        Druk daarna op ctrl+s om op te slaan.
</p></div>

<div class="details-image" id="main-image">
    <a href="/afbeelding/<?php echo $work->url_title ?>" target="_blank">
        <img src="<?php echo ImageHelper::preview($work->image)?>" alt="<?php echo $work->title?>">
    </a>
</div>

<?php
if (sizeof($all_details) > 0) {
    foreach($all_details as $details) {
        echo '<div class="details-image">'."\n";
        echo '    <a href="/afbeelding/'.$work->url_title.'/'.$details->id.'" target="_blank">'."\n";
        echo '         <img src="'.ImageHelper::preview($details->image).'" alt="'.$work->title.'">'."\n";
        echo '    </a>'."\n";
        echo '</div>'."\n\n";
    }
}
?>
<div class="details-menu">
    <a href="<?php echo $previous_url ?>">
        <div class="button details-button">
            Vorige
        </div>
    </a>
    <a href="/">
        <div class="button details-button">
            Terug
        </div>
    </a>
    <a href="<?php echo $next_url ?>">
        <div class="button details-button">
            Volgende
        </div>
    </a>
</div>
<div class="menu-spacer"><br></div>
<?php include('footer.php') ?>