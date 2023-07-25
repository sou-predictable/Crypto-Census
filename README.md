# Crypto Census

The aim of Crypto Census is to reduce the profitability of cryptojacking by providing organizations a centralized text list of known public mining pools and crypto domains. This list can then be applied as firewall exclusions, or IOC detections.

This project also aims to provide the community with a documented, customizable, self-maintainable aggregation tool for crypto-related domains.

## Status

Currently Crypto Census exists only as a proof-of-concept.

## How It Works

Crypto Census utilizes a `curl`-based web crawler and web scraper to:

1. Identify HTML documents.
2. Identify and follow ```<a href>``` HTML links.
3. Identify strings within the HTML document which may be domains.
4. Check extracted domains for crypto activity.
5. Output validated domains to a text list.

## Planned Features

Before a 1.0 release, here is a roadmap of planned changes/improvements:

* More Verbose Documentation
* Statically-Linked Binary
* Pooled `curl` connections