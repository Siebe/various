<?php include 'back_header.php';?>
<h3>Hoofdmenu</h3>

<p>Welkom <?php echo isset($username) ? ucfirst($username) : 'gebruiker'?>,

Kies een item in het menu hierboven om te bewerken óf kies een actie hieronder:</p>
    
<a href ="/backend/werk/edit">
    <div class ="button action-button">Nieuw werk toevoegen</div>
</a>
    
<a href ="/backend/nieuws/edit">
    <div class ="button action-button">Nieuws-item toevoegen</div>
</a>
    
<a href ="/" target="_blank">
    <div class ="button action-button">Open voorkant website</div>
</a>
    
<a href ="/backend/loguit">
    <div class ="button action-button">Uitloggen op deze computer</div>
</a>

<?php include 'footer.php';?>
