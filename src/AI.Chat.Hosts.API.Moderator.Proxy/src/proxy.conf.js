const target = 'http://localhost:5118';

const PROXY_CONFIG = [
  {
    context: [
      "/commands",
    ],
    target,
    secure: false
  }
]

module.exports = PROXY_CONFIG;
