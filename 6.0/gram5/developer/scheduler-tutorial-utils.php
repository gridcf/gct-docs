<?php
function doc_version()
{
    preg_match('/\/([^\/]+)\/execution/',
            $_SERVER[REQUEST_URI],
            $version);

    return $version[1];
}

function javadoc_path()
{
    return "/api/javadoc-" . doc_version();
}


function doxygen_path()
{
    return "/api/c-globus-" . doc_version();
}

function perldoc_path()
{
    return doxygen_path() . "/perl";
}

?>
