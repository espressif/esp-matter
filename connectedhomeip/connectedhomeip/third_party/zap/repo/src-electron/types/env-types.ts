export interface VersionType {
  version: string
  featureLevel: number
  hash: number
  timestamp: number
  date: string
  source?: boolean
}

export interface ErrorType {
  msg: string
  err?: {
    alert: string
  } | null
}
