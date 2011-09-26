<?php
/**
 * @file
 * This file contains conversion functions.
 */

DEFINE('SWIFTMAILER_DATE_PATTERN', '(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-9]{2} (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} (2[0-3]|[01][0-9]):([0-5][0-9]):([0-5][0-9]) (\+|\-)([01][0-2])([0-5][0-9])');
DEFINE('SWIFTMAILER_MAILBOX_PATTERN', '(.*\<.*@.*\>|.*@.*,)');

/**
 * Determines the header type based on the a header key and value.
 *
 * @param string $key
 *   The header key.
 * @param string $value
 *   The header value.
 *
 * @return string
 *   The header type as determined based on the provided header key
 *   and value.
 */
function swiftmailer_get_headertype($key, $value) {

    if (swiftmailer_is_parameterized_header($key, $value)) {
        return SWIFTMAILER_HEADER_PARAMETERIZED;
    }

    if (swiftmailer_is_date_header($key, $value)) {
        return SWIFTMAILER_HEADER_DATE;
    }

    if (swiftmailer_is_mailbox_header($key, $value)) {
        return SWIFTMAILER_HEADER_MAILBOX;
    }

    if (swiftmailer_is_id_header($key, $value)) {
        return SWIFTMAILER_HEADER_ID;
    }

    if (swiftmailer_is_path_header($key, $value)) {
        return SWIFTMAILER_HEADER_PATH;
    }

    return SWIFTMAILER_HEADER_TEXT;
    
}

/**
 * Adds a text header to a message.
 *
 * @param Swift_Message $message
 *   The message which the text header is to be added to.
 * @param string $key
 *   The header key.
 * @param string $value
 *   The header value.
 */
function swiftmailer_add_text_header(Swift_Message $message, $key, $value) {

    // Remove any already existing header identified by the provided key.
    swiftmailer_remove_header($message, $key);
    
    // Add the header.
    $message->getHeaders()->addTextHeader($key, $value);
}

/**
 * Checks whether a header is a parameterized header.
 *
 * @see http://swift_mailer.org/docs/header-parameterized
 *
 * @param string $key
 *   The header key.
 * @param string $value
 *   The header value.
 * @return boolean
 *   TRUE if the provided header is a parameterized header,
 *   and otherwise FALSE.
 */
function swiftmailer_is_parameterized_header($key, $value) {
    if (preg_match('/;/', $value)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * Adds a parameterized header to a message.
 *
 * @param Swift_Message $message
 *   The message which the parameterized header is to be added to.
 * @param string $key
 *   The header key.
 * @param string $value
 *   The header value.
 */
function swiftmailer_add_parameterized_header(Swift_Message $message, $key, $value) {

    // Remove any already existing header identified by the provided key.
    swiftmailer_remove_header($message, $key);

    // Define variables to hold the header's value and parameters.
    $header_value = NULL;
    $header_parameters = array();

    // Split the provided value by ';' (semicolon), which we assume is the
    // character is used to separate the parameters.
    $parameter_pairs = explode(';', $value);

    // Iterate through the extracted parameters, and prepare each of them to be
    // added to a parameterized message header. There should be a single text
    // parameter and one or more key/value parameters in the provided header
    // value. We assume that a '=' (equals) character is used to separate the
    // key and value for each of the parameters.
    foreach ($parameter_pairs as $parameter_pair) {

        // Find out whether the current parameter pair really is a parameter
        // pair or just a single value.
        if (preg_match('/=/', $parameter_pair) > 0) {

            // Split the parameter so that we can access the parameter's key and
            // value separately.
            $parameter_pair = explode('=', $parameter_pair);
            
            // Validate that the parameter has been split in two, and that both
            // the parameter's key and value is accessible. If that is the case,
            // then add the current parameter's key and value to the array which
            // holds all parameters to be added to the current header.
            if (!empty($parameter_pair[0]) && !empty($parameter_pair[1])) {
                $header_parameters[$parameter_pair[0]] = $parameter_pair[1];
            }

        } else {
            $header_value = $parameter_pair;
        }
    }

    // Add the parameterized header.
    $message->getHeaders()->addParameterizedHeader($key, $header_value, $header_parameters);

}

/**
 * Checks whether a header is a date header.
 *
 * @see http://swift_mailer.org/docs/header-date
 *
 * @param string $key
 *   The header key.
 * @param string $value
 *   The header value.
 * @return boolean
 *   TRUE if the provided header is a date header, and otherwise FALSE.
 */
function swiftmailer_is_date_header($key, $value) {
    if (preg_match('/' . SWIFTMAILER_DATE_PATTERN . '/', $value)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * Adds a date header to a message.
 *
 * @param Swift_Message $message
 *   The message which the date header is to be added to.
 * @param string $key
 *   The header key.
 * @param string $value
 *   The header value.
 */
function swift_mailer_add_date_header(Swift_Message $message, $key, $value) {

    // Remove any already existing header identified by the provided key.
    swiftmailer_remove_header($message, $key);

    // Add the header.
    $message->getHeaders()->addDateHeader($key, $value);
}

/**
 * Checks whether a header is a mailbox header.
 *
 * It is difficult to distinguish id, mailbox and path headers from each other
 * as they all may very well contain the exact same value. This function simply
 * checks whether the header key equals to 'Message-ID' to determine if the
 * header is a path header.
 *
 * @see http://swift_mailer.org/docs/header-mailbox
 *
 * @param string $key
 *   The header key.
 * @param string $value
 *   The header value.
 *
 * @return boolean
 *   TRUE if the provided header is a mailbox header, and otherwise FALSE.
 */
function swiftmailer_is_mailbox_header($key, $value) {
    if (preg_match('/' . SWIFTMAILER_MAILBOX_PATTERN . '/', $value)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * Adds a mailbox header to a message.
 *
 * @param Swift_Message $message
 *   The message which the mailbox header is to be added to.
 * @param string $key
 *   The header key.
 * @param string $value
 *   The header value.
 */
function swiftmailer_add_mailbox_header(Swift_Message $message, $key, $value) {

    // Remove any already existing header identified by the provided key.
    swiftmailer_remove_header($message, $key);

    // Initialise an array which will hold the mailboxes which are to be
    // added to the mailbox header.
    $header_mailboxes = array();

    // Split the provided value by ',' (comma), which we assume is the
    // character is used to separate the mailboxes.
    $mailboxes = explode(',', $value);

    // Iterate through the extracted mailboxes, and prepare each of them to be
    // added as a mailbox message header. There should one or more mailboxes
    // either represented as a single address or a pair of name and address.
    // Mailboxes represented as  a single address should be in the format
    // '[address]', while pairs of name and address should be in the format
    // '[name] <[address]>'.
    foreach ($mailboxes as $mailbox) {

        // Find out whether the current mailbox is a pair of name and address,
        // or just an address.
        if (preg_match('/<.*>/', $mailbox)) {
            $mailbox_components = explode('<', $mailbox);
            $mailbox_name = trim($mailbox_components[0]);
            $mailbox_address = preg_replace('/>.*/', '', $mailbox_components[1]);
            $header_mailboxes[$mailbox_address] = trim($mailbox_name);

        } else {
            $header_mailboxes[] = trim($mailbox);
        }
    }

    //Add the header.
    $message->getHeaders()->addMailboxHeader($key, $header_mailboxes);
}

/**
 * Checks whether a header is an id header.
 *
 * It is difficult to distinguish id, mailbox and path headers from each other
 * as they all may very well contain the exact same value. This function simply
 * checks whether the header key equals to 'Message-ID' to determine if the
 * header is a path header.
 *
 * @see http://swift_mailer.org/docs/header-id
 *
 * @param string $key
 *   The header key.
 * @param string $value
 *   The header value.
 *
 * @return boolean
 *   TRUE if the provided header is an id header, and otherwise FALSE.
 */
function swiftmailer_is_id_header($key, $value) {
    if (valid_email_address($value) && $key == 'Message-ID') {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * Adds an id header to a message.
 *
 * @param Swift_Message $message
 *   The message which the id header is to be added to.
 * @param string $key
 *   The header key.
 * @param string $value
 *   The header value.
 */
function swiftmailer_add_id_header(Swift_Message $message, $key, $value) {

    // Remove any already existing header identified by the provided key.
    swiftmailer_remove_header($message, $key);

    // Add the header.
    $message->getHeaders()->addIdHeader($key, $value);
}

/**
 * Checks whether a header is a path header.
 *
 * It is difficult to distinguish id, mailbox and path headers from each other
 * as they all may very well contain the exact same value. This function simply
 * checks whether the header key equals to 'Message-ID' to determine if the
 * header is a path header.
 *
 * @see http://swift_mailer.org/docs/header-path
 *
 * @param string $key
 *   The header key.
 * @param string $value
 *   The header value.
 * @return boolean
 *   TRUE if the provided header is a path header, and otherwise FALSE.
 */
function swiftmailer_is_path_header($key, $value) {
    if (valid_email_address($value) && $key == 'Return-Path') {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * Adds a path header to a message.
 *
 * @param Swift_Message $message
 *   The message which the path header is to be added to.
 *
 * @param string $key
 *   The header key.
 * @param string $value
 *   The header value.
 */
function swiftmailer_add_path_header(Swift_Message $message, $key, $value) {

    // Remove any already existing header identified by the provided key.
    swiftmailer_remove_header($message, $key);

    // Add the header.
    $message->getHeaders()->addPathHeader($key, $value);
}

/**
 * Removes a header from a message.
 * 
 * @param Swift_Message $message
 *   The message which the header is to be removed from.
 * @param string $key
 *   The header key.
 */
function swiftmailer_remove_header(Swift_Message $message, $key) {

    // Get message headers.
    $headers = $message->getHeaders();

    // Remove the header if it already exists.
    $headers->removeAll($key);

}