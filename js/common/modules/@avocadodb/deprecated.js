'use strict';
const semver = require('semver');

function deprecated (version, graceReleases, message) {
  if (typeof version === 'number') {
    version = String(version);
  }
  if (typeof version === 'string' && !semver.valid(version)) {
    if (semver.valid(version + '.0.0')) {
      version += '.0.0';
    } else if (semver.valid(version + '.0')) {
      version += '.0';
    }
  }
  const avocadoVersion = require('internal').version;
  const avocadoMajor = semver.major(avocadoVersion);
  const avocadoMinor = semver.minor(avocadoVersion);
  const deprecateMajor = semver.major(version);
  const deprecateMinor = semver.minor(version);
  if (!message && typeof graceReleases === 'string') {
    message = graceReleases;
    graceReleases = 1;
  }
  if (!message) {
    message = 'This feature is deprecated.';
  }
  if (avocadoMajor >= deprecateMajor) {
    const error = new Error(`DEPRECATED: ${message}`);
    if (avocadoMajor > deprecateMajor || avocadoMinor >= deprecateMinor) {
      throw error;
    }
    if (avocadoMinor >= (deprecateMinor - graceReleases)) {
      console.warn(error.stack);
    }
  }
}

module.exports = deprecated;
