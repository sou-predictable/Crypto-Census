# CryptoCensus

The aim of CryptoCensus is to reduce the profitability of cryptojacking by providing organizations a centralized text list of known public mining pools and crypto domains. This list can then be applied as firewall exclusions, or in detections.

This project also aims to provide the cybersecurity community with a documented, customizable, self-maintainable aggregation tool for crypto-related domains.

## Status

Currently CryptoCensus exists only as a proof-of-concept. The ```output.txt``` file currently contains example output after the proof-of-concept code was run for 12 hours, using the files ```sources.txt```, ```exclusions.txt```, and ```terms.txt``` as input.

## How It Works

CryptoCensus utilizes a cURL-based web crawler and web scraper to:

1. Identify HTML documents.
2. Identify and follow ```<a href>``` HTML links.
3. Identify strings within the HTML document which may be domains.
4. Validate the identified domains are crypto-related.
5. Output validated domains to a text list.

To build and compile, cURL 7.86 must exist within the same directory as CryptoCensus. This can be found here: https://curl.se/download.html

## Planned Features

Before a 1.0 release, here is a roadmap of planned changes/improvements:

* Multi-Threading
* Improved Parsing Speed
* Improved Cryptomining Domain Recognition
* Reduced False Positive Indicators
* More Verbose Documentation
* Statically-Linked Binary
