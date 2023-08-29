# Crypto Census

The aim of Crypto Census is to reduce the profitability of cryptojacking by providing organizations a centralized text list of known public mining pools and crypto domains. This list can then be applied in domain indicator detections, or firewall exclusions.

This project also aims to provide the community with a documented, customizable, self-maintainable aggregation tool for crypto-related domains.

## How It Works

Crypto Census utilizes a `curl`-based web crawler and web scraper to:

1. Identify HTML documents.
2. Identify and follow ```<a href>``` HTML links.
3. Identify strings within the HTML document which may be domains.
4. Check extracted domains for crypto activity.
5. Output validated domains to a text list.

## Usage
To run Crypto Census, download and unzip `Crypto-Census.zip`. Then, execute `CryptoCensus.exe`. Alternatively, the file `output.txt` can be used out of the box for domain indicator detections.

The current values in `sources.txt` come from the [minerstat exclusion list](https://minerstat.com/mining-pool-whitelist.txt). The file`output.txt` contains the output of the program after a few hours of execution using default settings.

In the case of network issues, or other disruptions to Crypto Census, the tool is designed in a way that allows users to feed the data in `output.txt` back to `sources.txt`. Doing so allows Crypto Census to pick up where it left off. 

## Planned Features

Here is a roadmap of planned changes/improvements:

* Statically-Linked Binary
* Better DoS prevention mechanisms
* Root domain exclusions
* Pooled `curl` connections
* Greater resilience to network disruptions 
* Test compatability with Linux and MacOS