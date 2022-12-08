// SPDX-License-Identifier: GPL-2.0

use proc_macro::{token_stream, TokenTree};

pub(crate) fn try_ident(it: &mut token_stream::IntoIter) -> Option<String> {
    if let Some(TokenTree::Ident(ident)) = it.next() {
        Some(ident.to_string())
    } else {
        None
    }
}

pub(crate) fn try_literal(it: &mut token_stream::IntoIter) -> Option<String> {
    if let Some(TokenTree::Literal(literal)) = it.next() {
        Some(literal.to_string())
    } else {
        None
    }
}

<<<<<<< HEAD
pub(crate) fn try_string(it: &mut token_stream::IntoIter) -> Option<String> {
    try_literal(it).and_then(|string| {
        if string.starts_with('\"') && string.ends_with('\"') {
            let content = &string[1..string.len() - 1];
            if content.contains('\\') {
                panic!("Escape sequences in string literals not yet handled");
            }
            Some(content.to_string())
        } else if string.starts_with("r\"") {
            panic!("Raw string literals are not yet handled");
=======
pub(crate) fn try_byte_string(it: &mut token_stream::IntoIter) -> Option<String> {
    try_literal(it).and_then(|byte_string| {
        if byte_string.starts_with("b\"") && byte_string.ends_with('\"') {
            Some(byte_string[2..byte_string.len() - 1].to_string())
>>>>>>> b7ba80a49124 (Commit)
        } else {
            None
        }
    })
}

pub(crate) fn expect_ident(it: &mut token_stream::IntoIter) -> String {
    try_ident(it).expect("Expected Ident")
}

pub(crate) fn expect_punct(it: &mut token_stream::IntoIter) -> char {
    if let TokenTree::Punct(punct) = it.next().expect("Reached end of token stream for Punct") {
        punct.as_char()
    } else {
        panic!("Expected Punct");
    }
}

<<<<<<< HEAD
pub(crate) fn expect_string(it: &mut token_stream::IntoIter) -> String {
    try_string(it).expect("Expected string")
}

pub(crate) fn expect_string_ascii(it: &mut token_stream::IntoIter) -> String {
    let string = try_string(it).expect("Expected string");
    assert!(string.is_ascii(), "Expected ASCII string");
    string
=======
pub(crate) fn expect_byte_string(it: &mut token_stream::IntoIter) -> String {
    try_byte_string(it).expect("Expected byte string")
>>>>>>> b7ba80a49124 (Commit)
}

pub(crate) fn expect_end(it: &mut token_stream::IntoIter) {
    if it.next().is_some() {
        panic!("Expected end");
    }
}
